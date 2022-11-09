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

# Lint as: python3
"""This file contains the python libraries for the regex_split op."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.framework import dtypes
from tensorflow.python.ops.ragged import ragged_tensor
from tensorflow.python.framework import load_library
from tensorflow.python.platform import resource_loader
gen_regex_split_ops = load_library.load_op_library(resource_loader.get_path_to_datafile('_regex_split_ops.so'))


# pylint: disable= redefined-builtin
def regex_split_with_offsets(input,
                             delim_regex_pattern,
                             keep_delim_regex_pattern="",
                             name=None):
  r"""Split `input` by delimiters that match a regex pattern; returns offsets.

  `regex_split_with_offsets` will split `input` using delimiters that match a
  regex pattern in `delim_regex_pattern`. Here is an example:

  ```
  text_input=["hello there"]
  # split by whitespace
  result, begin, end = regex_split_with_offsets(text_input, "\s")
  # result = [["hello", "there"]]
  # begin = [[0, 7]]
  # end = [[5, 11]]
  ```

  By default, delimiters are not included in the split string results.
  Delimiters may be included by specifying a regex pattern
  `keep_delim_regex_pattern`. For example:

  ```
  text_input=["hello there"]
  # split by whitespace
  result, begin, end = regex_split_with_offsets(text_input, "\s", "\s")
  # result = [["hello", " ", "there"]]
  # begin = [[0, 5, 7]]
  # end = [[5, 6, 11]]
  ```

  If there are multiple delimiters in a row, there are no empty splits emitted.
  For example:

  ```
  text_input=["hello  there"]  # two continuous whitespace characters
  # split by whitespace
  result, begin, end = regex_split_with_offsets(text_input, "\s")
  # result = [["hello", "there"]]
  ```

  See https://github.com/google/re2/wiki/Syntax for the full list of supported
  expressions.

  Args:
    input: A Tensor or RaggedTensor of string input.
    delim_regex_pattern: A string containing the regex pattern of a delimiter.
    keep_delim_regex_pattern: (optional) Regex pattern of delimiters that should
      be kept in the result.
    name: (optional) Name of the op.

  Returns:
    A tuple of RaggedTensors containing:
      (split_results, begin_offsets, end_offsets)
    where tokens is of type string, begin_offsets and end_offsets are of type
    int64.
  """
  delim_regex_pattern = b"".join(
      [b"(", delim_regex_pattern.encode("utf-8"), b")"])
  keep_delim_regex_pattern = b"".join(
      [b"(", keep_delim_regex_pattern.encode("utf-8"), b")"])

  # Convert input to ragged or tensor
  input = ragged_tensor.convert_to_tensor_or_ragged_tensor(
      input, dtype=dtypes.string)

  if ragged_tensor.is_ragged(input):
    # send flat_values to regex_split op.
    tokens, begin_offsets, end_offsets, row_splits = (
        gen_regex_split_ops.regex_split_with_offsets(
            input.flat_values,
            delim_regex_pattern,
            keep_delim_regex_pattern,
            name=name))

    # Pack back into original ragged tensor
    tokens_rt = ragged_tensor.RaggedTensor.from_row_splits(tokens, row_splits)
    tokens_rt = ragged_tensor.RaggedTensor.from_row_splits(
        tokens_rt, input.row_splits)
    begin_offsets_rt = ragged_tensor.RaggedTensor.from_row_splits(
        begin_offsets, row_splits)
    begin_offsets_rt = ragged_tensor.RaggedTensor.from_row_splits(
        begin_offsets_rt, input.row_splits)
    end_offsets_rt = ragged_tensor.RaggedTensor.from_row_splits(
        end_offsets, row_splits)
    end_offsets_rt = ragged_tensor.RaggedTensor.from_row_splits(
        end_offsets_rt, input.row_splits)
    return tokens_rt, begin_offsets_rt, end_offsets_rt

  else:
    # send flat_values to regex_split op.
    tokens, begin_offsets, end_offsets, row_splits = (
        gen_regex_split_ops.regex_split_with_offsets(input, delim_regex_pattern,
                                                     keep_delim_regex_pattern))

    # Pack back into ragged tensors
    tokens_rt = ragged_tensor.RaggedTensor.from_row_splits(
        tokens, row_splits=row_splits)
    begin_offsets_rt = ragged_tensor.RaggedTensor.from_row_splits(
        begin_offsets, row_splits=row_splits)
    end_offsets_rt = ragged_tensor.RaggedTensor.from_row_splits(
        end_offsets, row_splits=row_splits)
    return tokens_rt, begin_offsets_rt, end_offsets_rt


# pylint: disable= redefined-builtin
def regex_split(input,
                delim_regex_pattern,
                keep_delim_regex_pattern="",
                name=None):
  r"""Split `input` by delimiters that match a regex pattern.

  `regex_split` will split `input` using delimiters that match a
  regex pattern in `delim_regex_pattern`. Here is an example:

  ```
  text_input=["hello there"]
  # split by whitespace
  result, begin, end = regex_split_with_offsets(text_input, "\s")
  # result = [["hello", "there"]]

  ```

  By default, delimiters are not included in the split string results.
  Delimiters may be included by specifying a regex pattern
  `keep_delim_regex_pattern`. For example:

  ```
  text_input=["hello there"]
  # split by whitespace
  result, begin, end = regex_split_with_offsets(text_input, "\s", "\s")
  # result = [["hello", " ", "there"]]
  ```

  If there are multiple delimiters in a row, there are no empty splits emitted.
  For example:

  ```
  text_input=["hello  there"]  # two continuous whitespace characters
  # split by whitespace
  result, begin, end = regex_split_with_offsets(text_input, "\s")
  # result = [["hello", "there"]]
  ```

  See https://github.com/google/re2/wiki/Syntax for the full list of supported
  expressions.

  Args:
    input: A Tensor or RaggedTensor of string input.
    delim_regex_pattern: A string containing the regex pattern of a delimiter.
    keep_delim_regex_pattern: (optional) Regex pattern of delimiters that should
      be kept in the result.
    name: (optional) Name of the op.

  Returns:
    A RaggedTensors containing of type string containing the split string
    pieces.
  """
  tokens, _, _ = regex_split_with_offsets(input, delim_regex_pattern,
                                          keep_delim_regex_pattern, name)
  return tokens
