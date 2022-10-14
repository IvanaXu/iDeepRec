/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifdef INTEL_MKL

#include "tensorflow/core/common_runtime/kernel_benchmark_testlib.h"
#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_shape.h"
#include "tensorflow/core/framework/types.pb.h"
#include "tensorflow/core/graph/graph.h"
#include "tensorflow/core/graph/node_builder.h"
#include "tensorflow/core/graph/testlib.h"
#include "tensorflow/core/kernels/broadcast_to_op.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/test.h"
#include "tensorflow/core/platform/test_benchmark.h"
#include "tensorflow/core/public/session.h"

namespace tensorflow {
namespace {

Node* BatchMatmul(const string& kind, Graph* g, Node* in0, Node* in1, bool adj_x, bool adj_y) {
  Node* ret;
  const bool isDefault = (kind == "Default");
  string op_name = isDefault ? "BatchMatMul" : "_MklBatchMatMul";
  auto builder = NodeBuilder(g->NewName("n"), op_name)
                  .Input(in0)
                  .Input(in1)
                  .Attr("adj_x", adj_x)
                  .Attr("adj_y", adj_y);
  if( !isDefault ){
      builder.Attr("_kernel", "MklNameChangeOp");
  }
  TF_CHECK_OK(builder.Finalize(g, &ret));
  return ret;
}

template <typename T>
static Graph* BatchMatmul(const string& kind, int b, int m, int k, int n, bool adjoint_a,
                          bool adjoint_b) {
  Graph* g = new Graph(OpRegistry::Global());
  DataType type = DataTypeToEnum<T>::v();
  Tensor in0(type, adjoint_a ? TensorShape({b, k, m}) : TensorShape({b, m, k}));
  in0.flat<T>().setRandom();
  Tensor in1(type, adjoint_b ? TensorShape({b, n, k}) : TensorShape({b, k, n}));
  in1.flat<T>().setRandom();
  BatchMatmul(kind, g, test::graph::Constant(g, in0),
                           test::graph::Constant(g, in1), adjoint_a, adjoint_b);
  return g;
}

#define BM_BatchMatmul_Base(kind, B, M, K, N, TA, TB, T, DEVICE, NTH)                          \
  static void                                                                                  \
      BM_BatchMatmul##_##kind##_##B##_##M##_##K##_##N##_##TA##_##TB##_##T##_##DEVICE##_##NTH(  \
          int iters) {                                                                         \
    testing::UseRealTime();                                                                    \
    testing::ItemsProcessed(static_cast<int64>(iters) * B * M * K * N * 2);                    \
    SessionOptions opts;                                                                       \
    opts.config.set_intra_op_parallelism_threads(NTH);                                         \
    test::Benchmark(#DEVICE, BatchMatmul<T>(#kind, B, M, K, N, TA, TB), &opts).Run(iters);     \
  }                                                                                            \
  BENCHMARK(                                                                                   \
      BM_BatchMatmul##_##kind##_##B##_##M##_##K##_##N##_##TA##_##TB##_##T##_##DEVICE##_##NTH); \

#define BM_BatchMatmul_kind(B, M, K, N, TA, TB, T, DEVICE, NTH)     \
  BM_BatchMatmul_Base(Default, B, M, K, N, TA, TB, T, DEVICE, NTH); \
  BM_BatchMatmul_Base(Mkl, B, M, K, N, TA, TB, T, DEVICE, NTH);     \

#define BM_BatchMatmul_NTH(B, M, K, N, TA, TB, T, DEVICE) \
  BM_BatchMatmul_kind(B, M, K, N, TA, TB, T, DEVICE, 1);  \
  BM_BatchMatmul_kind(B, M, K, N, TA, TB, T, DEVICE, 4);  \
  BM_BatchMatmul_kind(B, M, K, N, TA, TB, T, DEVICE, 8);  \

#define BM_BatchMatmul(B, M, K, N, TA, TB)               \
  BM_BatchMatmul_NTH(B, M, K, N, TA, TB, float, cpu);    \
  BM_BatchMatmul_NTH(B, M, K, N, TA, TB, bfloat16, cpu); \

/*
// Typical fully connected layers
BM_BatchMatmul(1, 1, 1024, 1024, false, false);
BM_BatchMatmul(1, 8, 1024, 1024, false, false);
BM_BatchMatmul(1, 16, 1024, 1024, false, false);
BM_BatchMatmul(1, 128, 1024, 1024, false, false);
BM_BatchMatmul(2, 1, 1024, 1024, false, false);
BM_BatchMatmul(2, 8, 1024, 1024, false, false);
BM_BatchMatmul(2, 16, 1024, 1024, false, false);
BM_BatchMatmul(2, 128, 1024, 1024, false, false);
BM_BatchMatmul(8, 1, 1024, 1024, false, false);
BM_BatchMatmul(8, 8, 1024, 1024, false, false);
BM_BatchMatmul(8, 16, 1024, 1024, false, false);
BM_BatchMatmul(8, 128, 1024, 1024, false, false);
BM_BatchMatmul(32, 1, 1024, 1024, false, false);
BM_BatchMatmul(32, 8, 1024, 1024, false, false);
BM_BatchMatmul(32, 16, 1024, 1024, false, false);
BM_BatchMatmul(32, 128, 1024, 1024, false, false);

// Square matmul.
BM_BatchMatmul(1, 32, 32, 32, false, false);
BM_BatchMatmul(1, 128, 128, 128, false, false);
BM_BatchMatmul(1, 256, 256, 256, false, false);
BM_BatchMatmul(1, 1024, 1024, 1024, false, false);
BM_BatchMatmul(1, 2048, 2048, 2048, false, false);
BM_BatchMatmul(2, 32, 32, 32, false, false);
BM_BatchMatmul(2, 128, 128, 128, false, false);
BM_BatchMatmul(2, 256, 256, 256, false, false);
BM_BatchMatmul(2, 1024, 1024, 1024, false, false);
BM_BatchMatmul(2, 2048, 2048, 2048, false, false);
BM_BatchMatmul(4, 32, 32, 32, false, false);
BM_BatchMatmul(4, 128, 128, 128, false, false);
BM_BatchMatmul(4, 256, 256, 256, false, false);
BM_BatchMatmul(4, 1024, 1024, 1024, false, false);
BM_BatchMatmul(4, 2048, 2048, 2048, false, false);
BM_BatchMatmul(8, 32, 32, 32, false, false);
BM_BatchMatmul(8, 128, 128, 128, false, false);
BM_BatchMatmul(8, 256, 256, 256, false, false);
BM_BatchMatmul(8, 1024, 1024, 1024, false, false);
BM_BatchMatmul(8, 2048, 2048, 2048, false, false);
BM_BatchMatmul(32, 32, 32, 32, false, false);
BM_BatchMatmul(32, 128, 128, 128, false, false);
BM_BatchMatmul(32, 256, 256, 256, false, false);
BM_BatchMatmul(32, 1024, 1024, 1024, false, false);
BM_BatchMatmul(32, 2048, 2048, 2048, false, false);

// Matrix-vector multiplies.
BM_BatchMatmul(1, 10000, 200, 1, false, false);
BM_BatchMatmul(8, 10000, 200, 1, false, false);
BM_BatchMatmul(32, 10000, 200, 1, false, false);
BM_BatchMatmul(1, 10000, 200, 1, true, false);
BM_BatchMatmul(8, 10000, 200, 1, true, false);
BM_BatchMatmul(32, 10000, 200, 1, true, false);
BM_BatchMatmul(1, 10000, 200, 1, false, true);
BM_BatchMatmul(8, 10000, 200, 1, false, true);
BM_BatchMatmul(32, 10000, 200, 1, false, true);
BM_BatchMatmul(1, 10000, 200, 1, true, true);
BM_BatchMatmul(8, 10000, 200, 1, true, true);
BM_BatchMatmul(32, 10000, 200, 1, true, true);

// Vector-matrix multiplies.
BM_BatchMatmul(1, 1, 200, 10000, false, false);
BM_BatchMatmul(8, 1, 200, 10000, false, false);
BM_BatchMatmul(32, 1, 200, 10000, false, false);
BM_BatchMatmul(1, 1, 200, 10000, true, false);
BM_BatchMatmul(8, 1, 200, 10000, true, false);
BM_BatchMatmul(32, 1, 200, 10000, true, false);
BM_BatchMatmul(1, 1, 200, 10000, false, true);
BM_BatchMatmul(8, 1, 200, 10000, false, true);
BM_BatchMatmul(32, 1, 200, 10000, false, true);
BM_BatchMatmul(1, 1, 200, 10000, true, true);
BM_BatchMatmul(8, 1, 200, 10000, true, true);
BM_BatchMatmul(32, 1, 200, 10000, true, true);
*/

BM_BatchMatmul(8192,  32, 1, 48, false, true);
BM_BatchMatmul(8192,  48, 1, 32, false, false);

BM_BatchMatmul(8192,  1, 32, 48, false, true);
BM_BatchMatmul(8192,  1, 48, 32, false, false);

BM_BatchMatmul(8192,  48, 32, 1, false, true);
BM_BatchMatmul(8192,  32, 48, 1, false, false);

BM_BatchMatmul(8192, 50, 50, 16, false, false);
BM_BatchMatmul(8192, 50, 16, 50, false, true);
BM_BatchMatmul(8192, 50, 50, 16, true, false);

BM_BatchMatmul(8192, 48, 32, 48, false, true);
BM_BatchMatmul(8192, 48, 48, 32, false, false);
BM_BatchMatmul(8192, 48, 48, 32, true, false);


Node* BroadcastTo(Graph* g, Node* input, Node* shape) {
  Node* ret;
  TF_CHECK_OK(NodeBuilder(g->NewName("n"), "BroadcastTo")
                  .Input(input)
                  .Input(shape)
                  .Finalize(g, &ret));
  return ret;
}

Node* BatchMatmulV2(const string& kind, Graph* g, Node* in0, Node* in1, bool adj_x, bool adj_y) {
  Node* ret;
  const bool isDefault = (kind == "Default");
  string op_name = isDefault ? "BatchMatMulV2" : "_MklBatchMatMulV2";
  auto builder = NodeBuilder(g->NewName("n"), op_name)
                  .Input(in0)
                  .Input(in1)
                  .Attr("adj_x", adj_x)
                  .Attr("adj_y", adj_y);
  if( !isDefault ){
      builder.Attr("_kernel", "MklNameChangeOp");
  }
  TF_CHECK_OK(builder.Finalize(g, &ret));
  return ret;
}

template <typename T>
static Graph* BatchMatmulWithBroadcast(const string& kind, int b0, int b1, int m, int k, int n,
                                       bool manual_broadcast) {
  Graph* g = new Graph(OpRegistry::Global());
  DataType type = DataTypeToEnum<T>::v();
  Tensor in0(type, TensorShape({b0, m, k}));
  in0.flat<T>().setRandom();
  Tensor in1(type, TensorShape({b1, k, n}));
  in1.flat<T>().setRandom();

  Tensor broadcasted_in0_shape(DT_INT64, TensorShape({3}));
  Tensor broadcasted_in1_shape(DT_INT64, TensorShape({3}));

  Node* in0_node = nullptr;
  Node* in1_node = nullptr;
  if (manual_broadcast) {
    for (int i = 0; i < 3; ++i) {
      auto vec0 = broadcasted_in0_shape.vec<int64>();
      auto vec1 = broadcasted_in1_shape.vec<int64>();
      vec0(i) = (i == 0 ? std::max(b0, b1) : in0.shape().dim_size(i));
      vec1(i) = (i == 0 ? std::max(b0, b1) : in1.shape().dim_size(i));
    }
    in0_node = BroadcastTo(g, test::graph::Constant(g, in0),
                           test::graph::Constant(g, broadcasted_in0_shape));
    in1_node = BroadcastTo(g, test::graph::Constant(g, in1),
                           test::graph::Constant(g, broadcasted_in1_shape));
  } else {
    in0_node = test::graph::Constant(g, in0);
    in1_node = test::graph::Constant(g, in1);
  }

  BatchMatmulV2(kind, g, in0_node, in1_node, false, false);
  return g;
}


// Macro arguments names: --------------------------------------------------- //
//   B1: batch size of LHS
//   B2: batch size of RHS
//    M: outer dimension of LHS
//    K: inner dimensions of LHS and RHS
//    N: outer dimension of RHS
//   MB: boolean indicating whether to use manual broadcasting
//    T: C++ type of scalars (e.g. float, std::complex)
//    D: Device (e.g. cpu, gpu)
#define BM_BatchMatmulBCast_Base(kind, B1, B2, M, K, N, MB, T, DEVICE, NTH)                          \
  static void                                                                                        \
      BM_BatchMatmulBCast##_##kind##_##B1##_##B2##_##M##_##K##_##N##_##MB##_##T##_##DEVICE##_##NTH(  \
          int iters) {                                                                               \
    testing::UseRealTime();                                                                          \
    testing::ItemsProcessed(static_cast<int64>(iters) * std::max(B1, B2) * M * K * N * 2);           \
    SessionOptions opts;                                                                             \
    opts.config.set_intra_op_parallelism_threads(NTH);                                               \
    test::Benchmark(#DEVICE, BatchMatmulWithBroadcast<T>(#kind, B1, B2, M, K, N, MB), &opts)         \
        .Run(iters);                                                                                 \
  }                                                                                                  \
  BENCHMARK(                                                                                         \
      BM_BatchMatmulBCast##_##kind##_##B1##_##B2##_##M##_##K##_##N##_##MB##_##T##_##DEVICE##_##NTH); \

#define BM_BatchMatmulBCast_kind(B1, B2, M, K, N, MB, T, D, NTH)     \
  BM_BatchMatmulBCast_Base(Default, B1, B2, M, K, N, MB, T, D, NTH); \
  BM_BatchMatmulBCast_Base(Mkl, B1, B2, M, K, N, MB, T, D, NTH);     \

#define BM_BatchMatmulBCast_NTH(B1, B2, M, K, N, MB, T, DEVICE) \
  BM_BatchMatmulBCast_kind(B1, B2, M, K, N, MB, T, DEVICE, 1);  \
  BM_BatchMatmulBCast_kind(B1, B2, M, K, N, MB, T, DEVICE, 4);  \
  BM_BatchMatmulBCast_kind(B1, B2, M, K, N, MB, T, DEVICE, 8);  \

#define BM_BatchMatmulBCast(B1, B2, M, K, N, MB)               \
  BM_BatchMatmulBCast_NTH(B1, B2, M, K, N, MB, float, cpu);    \
  BM_BatchMatmulBCast_NTH(B1, B2, M, K, N, MB, bfloat16, cpu); \

/*
// Typical fully connected layers
BM_BatchMatmulBCast(1, 128, 1, 1024, 1024, true);
BM_BatchMatmulBCast(1, 128, 1, 1024, 1024, false);
BM_BatchMatmulBCast(128, 1, 1, 1024, 1024, true);
BM_BatchMatmulBCast(128, 1, 1, 1024, 1024, false);
BM_BatchMatmulBCast(1, 128, 128, 1024, 1024, true);
BM_BatchMatmulBCast(1, 128, 128, 1024, 1024, false);
BM_BatchMatmulBCast(128, 1, 128, 1024, 1024, true);
BM_BatchMatmulBCast(128, 1, 128, 1024, 1024, false);

// Square matmul.
BM_BatchMatmulBCast(1, 128, 512, 512, 512, true);
BM_BatchMatmulBCast(1, 128, 512, 512, 512, false);
BM_BatchMatmulBCast(128, 1, 512, 512, 512, true);
BM_BatchMatmulBCast(128, 1, 512, 512, 512, false);
BM_BatchMatmulBCast(1, 128, 1024, 1024, 1024, true);
BM_BatchMatmulBCast(1, 128, 1024, 1024, 1024, false);
BM_BatchMatmulBCast(128, 1, 1024, 1024, 1024, true);
BM_BatchMatmulBCast(128, 1, 1024, 1024, 1024, false);

// Matrix-vector multiplies.
BM_BatchMatmulBCast(1, 128, 10000, 200, 1, true);
BM_BatchMatmulBCast(1, 128, 10000, 200, 1, false);
BM_BatchMatmulBCast(128, 1, 10000, 200, 1, true);
BM_BatchMatmulBCast(128, 1, 10000, 200, 1, false);

// Vector-matrix multiplies.
BM_BatchMatmulBCast(1, 128, 1, 200, 10000, true);
BM_BatchMatmulBCast(1, 128, 1, 200, 10000, false);
BM_BatchMatmulBCast(128, 1, 1, 200, 10000, true);
BM_BatchMatmulBCast(128, 1, 1, 200, 10000, false);
*/

}  // namespace
}  // namespace tensorflow

#endif  // INTEL_MKL
