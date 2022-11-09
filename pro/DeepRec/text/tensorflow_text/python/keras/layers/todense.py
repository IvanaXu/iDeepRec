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

"""ToDense layer implementation to support composite tensors in keras models.

Implements the ToDense Keras layer that's to be used in feeding composite
tensors to recurrent layers or embeddings.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.framework import ops
from tensorflow.python.framework import sparse_tensor
from tensorflow.python.keras import layers
from tensorflow.python.keras.engine.base_layer import Layer
from tensorflow.python.ops import sparse_ops
from tensorflow.python.ops.ragged import ragged_tensor


class ToDense(Layer):   # pylint: disable=g-classes-have-attributes
  """Layer that makes padding and masking a Composite Tensors effortless.

  The layer takes a RaggedTensor or a SparseTensor and converts it to a uniform
  tensor by right-padding it or filling in missing values.

  Example:

  ```python
  x = tf.keras.layers.Input(shape=(None, None), ragged=True)
  y = tf_text.keras.layers.ToDense(mask=True)(x)
  model = tf.keras.Model(x, y)

  rt = tf.RaggedTensor.from_nested_row_splits(
    flat_values=[10, 11, 12, 13, 14, 15, 16, 17, 18, 19],
    nested_row_splits=([0, 1, 1, 5], [0, 3, 3, 5, 9, 10]))
  model.predict(rt)

  [[[10, 11, 12,  0], [ 0,  0,  0,  0], [ 0,  0,  0,  0], [ 0,  0,  0,  0]],
   [[ 0,  0,  0,  0], [ 0,  0,  0,  0], [ 0,  0,  0,  0], [ 0,  0,  0,  0]],
   [[ 0,  0,  0,  0], [13, 14,  0,  0], [15, 16, 17, 18], [19,  0,  0,  0]]]
  ```

  Arguments:
    pad_value: A value used to pad and fill in the missing values. Should be a
      meaningless value for the input data. Default is '0'.
    mask: A Boolean value representing whether to mask the padded values. If
      true, no any downstream Masking layer or Embedding layer with
      mask_zero=True should be added. Default is 'False'.
    **kwargs: kwargs of parent class.
  Input shape: Any Ragged or Sparse Tensor is accepted, but it requires the type
    of input to be specified via the Input or InputLayer from the Keras API.
  Output shape: The output is a uniform tensor having the same shape, in case of
    a ragged input or the same dense shape, in case of a sparse input.
  """

  def __init__(self, pad_value=0, mask=False, **kwargs):
    self._pad_value = pad_value
    self._mask = mask
    self._compute_output_and_mask_jointly = True
    self.trainable = False
    self.masking_layer = layers.Masking(mask_value=self._pad_value)

    super(ToDense, self).__init__(**kwargs)

  def call(self, inputs):
    if isinstance(inputs, ragged_tensor.RaggedTensor):
      # Convert the ragged tensor to a padded uniform tensor
      outputs = inputs.to_tensor(default_value=self._pad_value)
    elif isinstance(inputs, sparse_tensor.SparseTensor):
      # Fill in the missing value in the sparse_tensor
      outputs = sparse_ops.sparse_tensor_to_dense(
          inputs, default_value=self._pad_value)
    elif isinstance(inputs, ops.Tensor):
      outputs = inputs
    else:
      raise TypeError('Unexpected tensor type %s' % type(inputs).__name__)

    if self._mask:
      outputs = self.masking_layer(outputs)

    return outputs

  def compute_output_shape(self, input_shape):
    return input_shape

  def get_config(self):
    config = {'pad_value': self._pad_value, 'mask': self._mask}
    base_config = super(ToDense, self).get_config()
    return dict(list(base_config.items()) + list(config.items()))
