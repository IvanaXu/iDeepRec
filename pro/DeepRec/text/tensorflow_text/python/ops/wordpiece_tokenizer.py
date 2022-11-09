# coding=utf-8
# Copyright 2019 TF.Text Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Ops to tokenize words into subwords."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.compat import compat
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import ops
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import lookup_ops
from tensorflow.python.ops.ragged import ragged_tensor
from tensorflow.python.ops.ragged.ragged_tensor import RaggedTensor
from tensorflow_text.python.ops.tokenization import TokenizerWithOffsets

# pylint: disable=g-bad-import-order
from tensorflow.python.framework import load_library
from tensorflow.python.platform import resource_loader
gen_wordpiece_tokenizer = load_library.load_op_library(resource_loader.get_path_to_datafile('_wordpiece_tokenizer.so'))


class WordpieceTokenizer(TokenizerWithOffsets):
  """Tokenizes a tensor of UTF-8 string tokens into subword pieces."""

  def __init__(self,
               vocab_lookup_table,
               suffix_indicator='##',
               max_bytes_per_word=100,
               max_chars_per_token=None,
               token_out_type=dtypes.int64,
               unknown_token='[UNK]',
               split_unknown_characters=False):
    """Initializes the WordpieceTokenizer.

    Args:
      vocab_lookup_table: A lookup table implementing the LookupInterface
        containing the vocabulary of subwords.
      suffix_indicator: (optional) The characters prepended to a wordpiece to
        indicate that it is a suffix to another subword. Default is '##'.
      max_bytes_per_word: (optional) Max size of input token. Default is 100.
      max_chars_per_token: (optional) Max size of subwords, excluding suffix
        indicator. If known, providing this improves the efficiency of decoding
        long words.
      token_out_type: (optional) The type of the token to return. This can be
        `tf.int64` IDs, or `tf.string` subwords. The default is `tf.int64`.
      unknown_token: (optional) The value to use when an unknown token is found.
        Default is "[UNK]". If this is set to a string, and `token_out_type` is
        `tf.int64`, the `vocab_lookup_table` is used to convert the
        `unknown_token` to an integer. If this is set to `None`,
        out-of-vocabulary tokens are left as is.
      split_unknown_characters: (optional) Whether to split out single unknown
        characters as subtokens. If False (default), words containing unknown
        characters will be treated as single unknown tokens.
    """
    self._vocab_lookup_table = vocab_lookup_table
    self._suffix_indicator = suffix_indicator
    self._max_bytes_per_word = max_bytes_per_word
    self._max_chars_per_token = (
        0 if max_chars_per_token is None
        else max_chars_per_token)
    self._token_out_type = token_out_type
    self._unknown_token = unknown_token if unknown_token else '[UNK]'
    self._use_unknown_token = True if unknown_token else False
    self._split_unknown_characters = split_unknown_characters

  def tokenize(self, input):  # pylint: disable=redefined-builtin
    """Tokenizes a tensor of UTF-8 string tokens further into subword tokens.

    ### Example:
    ```python
    >>> tokens = [["they're", "the", "greatest"]],
    >>> tokenizer = WordpieceTokenizer(vocab, token_out_type=tf.string)
    >>> tokenizer.tokenize(tokens)
    [[['they', "##'", '##re'], ['the'], ['great', '##est']]]
    ```

    Args:
      input: An N-dimensional `Tensor` or `RaggedTensor` of UTF-8 strings.

    Returns:
      A `RaggedTensor` of tokens where `tokens[i1...iN, j]` is the string
      contents (or ID in the vocab_lookup_table representing that string)
      of the `jth` token in `input[i1...iN]`
    """
    subword, _, _ = self.tokenize_with_offsets(input)
    return subword

  def tokenize_with_offsets(self, input):  # pylint: disable=redefined-builtin
    """Tokenizes a tensor of UTF-8 string tokens further into subword tokens.

    ### Example:

    ```python
    >>> tokens = [["they're", "the", "greatest"]],
    >>> tokenizer = WordpieceTokenizer(vocab, token_out_type=tf.string)
    >>> result = tokenizer.tokenize_with_offsets(tokens)
    >>> result[0].to_list()  # subwords
    [[['they', "##'", '##re'], ['the'], ['great', '##est']]]
    >>> result[1].to_list()  # offset starts
    [[[0, 4, 5], [0], [0, 5]]]
    >>> result[2].to_list()  # offset limits
    [[[4, 5, 7], [3], [5, 8]]]
    ```

    Args:
      input: An N-dimensional `Tensor` or `RaggedTensor` of UTF-8 strings.

    Returns:
      A tuple `(tokens, start_offsets, limit_offsets)` where:

        * `tokens[i1...iN, j]` is a `RaggedTensor` of the string contents (or ID
          in the vocab_lookup_table representing that string) of the `jth` token
          in `input[i1...iN]`.
        * `start_offsets[i1...iN, j]` is a `RaggedTensor` of the byte offsets
          for the start of the `jth` token in `input[i1...iN]`.
        * `limit_offsets[i1...iN, j]` is a `RaggedTensor` of the byte offsets
          for the end of the `jth` token in `input[i`...iN]`.
    """
    name = None
    if not isinstance(self._vocab_lookup_table, lookup_ops.LookupInterface):
      raise TypeError('vocab_lookup_table must be a LookupInterface')
    with ops.name_scope(
        name, 'WordpieceTokenizeWithOffsets',
        [input, self._vocab_lookup_table, self._suffix_indicator]):
      # Check that the types are expected and the ragged rank is appropriate.
      tokens = ragged_tensor.convert_to_tensor_or_ragged_tensor(input)
      rank = tokens.shape.ndims
      if rank is None:
        raise ValueError('input must have a known rank.')

      if rank == 0:
        wordpieces, starts, limits = self.tokenize_with_offsets(
            array_ops.stack([tokens]))
        return wordpieces.values, starts.values, limits.values

      elif rank > 1:
        if not ragged_tensor.is_ragged(tokens):
          tokens = ragged_tensor.RaggedTensor.from_tensor(
              tokens, ragged_rank=rank - 1)
        wordpieces, starts, limits = self.tokenize_with_offsets(
            tokens.flat_values)
        wordpieces = wordpieces.with_row_splits_dtype(tokens.row_splits.dtype)
        starts = starts.with_row_splits_dtype(tokens.row_splits.dtype)
        limits = limits.with_row_splits_dtype(tokens.row_splits.dtype)
        return (tokens.with_flat_values(wordpieces),
                tokens.with_flat_values(starts),
                tokens.with_flat_values(limits))

      if compat.forward_compatible(2019, 8, 25):
        kwargs = dict(output_row_partition_type='row_splits')
        from_row_partition = RaggedTensor.from_row_splits
      else:
        kwargs = {}
        from_row_partition = RaggedTensor.from_row_lengths

      # Tokenize the tokens into subwords
      values, row_splits, starts, limits = (
          gen_wordpiece_tokenizer.wordpiece_tokenize_with_offsets(
              input_values=tokens,
              vocab_lookup_table=self._vocab_lookup_table.resource_handle,
              suffix_indicator=self._suffix_indicator,
              use_unknown_token=self._use_unknown_token,
              max_bytes_per_word=self._max_bytes_per_word,
              max_chars_per_token=self._max_chars_per_token,
              unknown_token=self._unknown_token,
              split_unknown_characters=self._split_unknown_characters,
              **kwargs))

      # If ids are desired, look them up in the vocab table. Otherwise just
      # return the string values.
      if self._token_out_type == dtypes.int64:
        values = self._vocab_lookup_table.lookup(values)

      wordpieces = from_row_partition(values, row_splits, validate=False)
      starts = from_row_partition(starts, row_splits, validate=False)
      limits = from_row_partition(limits, row_splits, validate=False)

      return wordpieces, starts, limits
