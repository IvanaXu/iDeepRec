/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

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

#include "absl/strings/match.h"
#include "mkldnn.hpp"
#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/math_ops.h"
#include "tensorflow/cc/ops/nn_ops.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/common_runtime/kernel_benchmark_testlib.h"
#include "tensorflow/core/framework/fake_input.h"
#include "tensorflow/core/framework/node_def_builder.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/types.pb.h"
#include "tensorflow/core/kernels/ops_testutil.h"
#include "tensorflow/core/kernels/ops_util.h"
#include "tensorflow/core/platform/cpu_info.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/stacktrace_handler.h"
#include "tensorflow/core/platform/str_util.h"
#include "tensorflow/core/platform/test.h"
#include "tensorflow/core/platform/test_benchmark.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/util/mkl_util.h"

// This is a special case, because EIGEN kernels does not have Swish Kerenls.
// Compare the performance of default tensorflow kernels (Eigen) with
// MKL kernels on CPU.
// Before running benchmarks, you need to configure OpenMP environment:
//   export TF_DISABLE_MKL=1 //To avoid Eigen kernels are rewrote by MKL.
//   export KMP_BLOCKTIME=0
//   export OMP_NUM_THREADS=${num_threads}
//   export KMP_AFFINITY=granularity=fine,verbose,compact,1,0
//
// Then you could use below command to test mkl and eigen performance:
// $bazel run --config opt --config=mkl \
//  //tensorflow/core/kernels/mkl:mkl_eltwise_swish_op_test -- --benchmarks=..
//
// ===========================================================================
// If you want to test MKL kernels accuracy with Eigen kernels, you need:
//   export TF_DISABLE_MKL=1 // To avoid Eigen kernels are rewrote by MKL.
// $bazel run --config opt --config=mkl \
//     //tensorflow/core/kernels/mkl:mkl_eltwise_swish_op_test

namespace tensorflow {

// --------------------------------------------------------------------------//
//  Test Swish Kernels accuracy and performance                              //
// --------------------------------------------------------------------------//
static const uint8 dummy_tensor[] = {0, 0, 0, 0, 0, 0, 0, 0};
static const TensorShape dummy_shape({8});

using GraphRunner = std::function<void(const Tensor& input, Tensor* output)>;

template <typename T>
class CommonTestUtilities : public OpsTestBase {
 public:
  void PerformConversion(DataType dtype, const Tensor& tensor,
                         const Tensor& mkl_meta_tensor, Tensor* output) {
    // Create an MKL to TF conversion node and execute it
    TF_EXPECT_OK(NodeDefBuilder("mkl_to_tf_op", "_MklToTf")
                     .Input(FakeInput(dtype))     // Input
                     .Input(FakeInput(DT_UINT8))  // Mkl second tensor
                     .Attr("T", dtype)
                     .Attr("_kernel", "MklLayoutDependentOp")
                     .Finalize(node_def()));
    TF_EXPECT_OK(InitOp());
    AddInputFromArray<T>(tensor.shape(), tensor.flat<T>());
    AddInputFromArray<uint8>(mkl_meta_tensor.shape(),
                             mkl_meta_tensor.flat<uint8>());
    TF_ASSERT_OK(RunOpKernel());

    *output = *GetOutput(0);
  }

  // Runs a Tensorflow graph defined by the root scope, and fetches the result
  // of 'fetch' node into the output Tensor.
  static void RunAndFetch(const tensorflow::Scope& root, const string& fetch,
                          Tensor* output) {
    tensorflow::GraphDef graph;
    TF_ASSERT_OK(root.ToGraphDef(&graph));

    // We really want to make sure that graph executed exactly as we passed it
    // to the session, so we disable various optimizations.
    tensorflow::SessionOptions session_options;

    // Disable common runtime constant folding.
    session_options.config.mutable_graph_options()
        ->mutable_optimizer_options()
        ->set_opt_level(OptimizerOptions::L0);

    // Disable Grappler optimizations for tests.
    tensorflow::RewriterConfig* cfg =
        session_options.config.mutable_graph_options()
            ->mutable_rewrite_options();
    cfg->set_constant_folding(tensorflow::RewriterConfig::OFF);
    cfg->set_layout_optimizer(tensorflow::RewriterConfig::OFF);
    cfg->set_remapping(tensorflow::RewriterConfig::OFF);

    std::unique_ptr<tensorflow::Session> session(
        tensorflow::NewSession(session_options));

    TF_ASSERT_OK(session->Create(graph));

    std::vector<Tensor> output_tensors;
    TF_ASSERT_OK(session->Run({}, {fetch}, {}, &output_tensors));

    *output = output_tensors[0];
  }

  void TestBody() {}

  static void VerifyTensorsClose(const GraphRunner& run,
                                 const GraphRunner& run_mkl) {
    int batch = 1;
    int height = 1;
    int width = 6;
    int depth = 2;
    float atol = 1e-6, rtol = 1e-6;
    DataType dtype = DataTypeToEnum<T>::v();

    Tensor input(dtype, {batch, height, width, depth});
    test::FillValues<T>(&input, {-13.86, -6.86, -2.51, -1.51, -0.53, 0.00, 0.53,
                                 1.51, 2.51, 6.10, 6.86, 13.86});
    Tensor output;
    Tensor mkl_output;
    run(input, &output);
    run_mkl(input, &mkl_output);

    ASSERT_EQ(output.dtype(), mkl_output.dtype());
    ASSERT_EQ(output.shape(), mkl_output.shape());
    if (dtype == DT_BFLOAT16) {
      rtol = 1e-2;
      atol = 1e-2;
    }
    test::ExpectClose(output, mkl_output, atol, rtol);
  }
};

template <typename T>
class SwishOpsTest : public OpsTestBase {
 protected:
  void VerifyReluOps() {
    const GraphRunner run = [this](const Tensor& input, Tensor* output) {
      auto root = tensorflow::Scope::NewRootScope();
      auto input_op =
          ops::Const(root.WithOpName("input"), Input::Initializer(input));
      auto sigmoid_op = ops::Sigmoid(root.WithOpName("Sigmoid"), input_op);
      auto mul_op = ops::Mul(root.WithOpName("Mul"), sigmoid_op, input_op);
      auto identity_op = ops::Identity(root.WithOpName("output"), mul_op);
      CommonTestUtilities<T>::RunAndFetch(root, "output", output);
    };

    const GraphRunner run_mkl = [this](const Tensor& input, Tensor* output) {
      DataType dtype = DataTypeToEnum<T>::v();

      TF_EXPECT_OK(NodeDefBuilder("Mkl_Swish", "_MklSwish")
                       .Input(FakeInput(dtype))
                       .Input(FakeInput(DT_UINT8))
                       .Attr("T", dtype)
                       .Attr("_kernel", "MklLayoutDependentOp")
                       .Finalize(node_def()));
      TF_EXPECT_OK(InitOp());

      AddInputFromArray<T>(input.shape(), input.flat<T>());
      AddInputFromArray<uint8>(dummy_shape, dummy_tensor);
      TF_ASSERT_OK(RunOpKernel());

      CommonTestUtilities<T> test_util;
      test_util.PerformConversion(dtype, *GetOutput(0), *GetOutput(1), output);
    };

    CommonTestUtilities<T>::VerifyTensorsClose(run, run_mkl);
  }
};

TYPED_TEST_SUITE_P(SwishOpsTest);

TYPED_TEST_P(SwishOpsTest, SwishTest) { this->VerifyReluOps(); }
REGISTER_TYPED_TEST_SUITE_P(SwishOpsTest, SwishTest);
using SwishOpsDataTypes = ::testing::Types<float, bfloat16>;

INSTANTIATE_TYPED_TEST_SUITE_P(Test, SwishOpsTest, SwishOpsDataTypes);

// --------------------------------------------------------------------------//
//  Test Swish Kernels accuracy and performance                              //
// --------------------------------------------------------------------------//
template <typename T>
static Graph* SwishGraph(const string& kind, const TensorShape& shape) {
  auto* graph = new Graph(OpRegistry::Global());

  DataType dtype = DataTypeToEnum<T>::v();
  Tensor input_t(dtype, shape);
  input_t.flat<T>().setRandom();
  Node* input = test::graph::Constant(graph, input_t, "input");
  Node* not_mkl_shape =
      test::graph::Constant(graph, GetMklMetaTensor(), "not_mkl");
  const bool isDefault = (kind == "Default");

  Node* sigmoid;
  Node* mul;
  Node* swish;
  if (isDefault) {
    TF_CHECK_OK(NodeBuilder(graph->NewName("Default_sigmoid"), "Sigmoid")
                    .Input(input)
                    .Attr("T", dtype)
                    .Finalize(graph, &sigmoid));

    TF_CHECK_OK(NodeBuilder(graph->NewName("Default_mul"), "Mul")
                    .Input(input)
                    .Input(sigmoid)
                    .Attr("T", dtype)
                    .Finalize(graph, &mul));
    return graph;
  }
  // Mkl Swish op.
  TF_CHECK_OK(NodeBuilder(graph->NewName("Mkl_swish"), "_MklSwish")
                  .Input(input)
                  .Input(not_mkl_shape)
                  .Attr("T", dtype)
                  .Attr("_kernel", "MklLayoutDependentOp")
                  .Finalize(graph, &swish));
  return graph;
}

#define BM_SWISH(kind, A, B, C, D, type, T)                                \
  static void BM_SWISH_##kind##_##type##_##A##_##B##_##C##_##D##_##T(      \
      int iters) {                                                         \
    int64 num_computed_elements = (A) * (B) * (C) * (D);                   \
    int64 flops_per_iter = num_computed_elements;                          \
    testing::ItemsProcessed(static_cast<int64>(iters) * flops_per_iter);   \
                                                                           \
    test::Benchmark(#type, SwishGraph<T>(#kind, {A, B, C, D})).Run(iters); \
  }                                                                        \
  BENCHMARK(BM_SWISH_##kind##_##type##_##A##_##B##_##C##_##D##_##T)

#define BENCHMARK_SWISH(A, B, C, D, type, T) \
  BM_SWISH(Default, A, B, C, D, type, T);    \
  BM_SWISH(Mkl, A, B, C, D, type, T);

#define BENCHMARK_DTYPE(T)                  \
  BENCHMARK_SWISH(1, 16, 16, 3, cpu, T);    \
  BENCHMARK_SWISH(16, 32, 32, 1, cpu, T);   \
  BENCHMARK_SWISH(16, 64, 64, 128, cpu, T); \
  BENCHMARK_SWISH(32, 64, 64, 128, cpu, T);

BENCHMARK_DTYPE(float)
BENCHMARK_DTYPE(bfloat16)

}  // namespace tensorflow

#endif  // INTEL_MKL
