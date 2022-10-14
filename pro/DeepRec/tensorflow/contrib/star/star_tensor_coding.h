#ifndef TENSORFLOW_CONTRIB_STAR_STAR_TENSOR_CODING_H_
#define TENSORFLOW_CONTRIB_STAR_STAR_TENSOR_CODING_H_

#include "tensorflow/core/common_runtime/device.h"
#include "tensorflow/core/distributed_runtime/call_options.h"
#include "tensorflow/core/framework/allocator.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/types.h"


namespace tensorflow {

struct StarBuf {
  uint64_t len_ = 0;
  char *data_ = nullptr;
  bool owned_ = true;
};

class StarTensorResponse {
public:
  virtual ~StarTensorResponse() {}

  void SetIsDead(bool is_dead) { is_dead_ = is_dead;}
  bool GetIsDead() const { return is_dead_; }

  // for dst device
  void InitAlloc(Device* d, const AllocatorAttributes& aa);
  Allocator* GetAlloc() { return allocator_; }
  AllocatorAttributes GetAllocAttributes() {return alloc_attrs_;}
  Device* GetDevice() const { return device_; }
  bool GetOnHost() const { return on_host_; }

  void SetTensor(const Tensor& tensor) { tensor_ = tensor; }
  const Tensor& GetTensor() const { return tensor_; }

  TensorProto& GetTensorProto() { return tensor_proto_; }

  void Clear();

  void SetDataType(DataType data_type) { data_type_ = data_type; }
  DataType GetDataType() { return data_type_; }

 private:
  bool is_dead_ = false;
  bool on_host_ = false;

  // for dst device
  Device* device_ = nullptr;
  AllocatorAttributes alloc_attrs_;
  Allocator* allocator_ = nullptr;

  Tensor tensor_;
  TensorProto tensor_proto_;
  DataType data_type_;
};

class StarFuseTensorResponse : public StarTensorResponse {
 public:
  StarFuseTensorResponse() : fuse_count_(0) {}
  virtual ~StarFuseTensorResponse() {}

  void Init(int fuse_count)  {
    fuse_count_ = fuse_count;
    tensors_.resize(fuse_count_);
    tensor_protos_.resize(fuse_count_);
    data_types_.resize(fuse_count_);
    is_deads_.resize(fuse_count_);
  }

  int GetFuseCount() { return fuse_count_; }

  void SetIsDeadByIndex(int idx, bool is_dead) { is_deads_[idx] = is_dead; }
  bool GetIsDeadByIndex(int idx) const { return is_deads_[idx]; }
  const std::vector<bool>& GetIsDeads() const { return is_deads_; }

  void SetTensorByIndex(int idx, const Tensor& tensor) { tensors_[idx] = tensor; }
  const Tensor& GetTensorByIndex(int idx) const { return tensors_[idx]; }
  const std::vector<Tensor>& GetTensors() const { return tensors_; }

  TensorProto& GetTensorProtoByIndex(int idx) { return tensor_protos_[idx]; }

  void Clear();

  void SetDataTypeByIndex(int idx, DataType data_type) { data_types_[idx] = data_type; }
  DataType GetDataTypeByIndex(int idx) { return data_types_[idx]; }

 private:
  int fuse_count_;
  std::vector<Tensor> tensors_;
  std::vector<TensorProto> tensor_protos_;
  std::vector<DataType> data_types_;
  std::vector<bool> is_deads_;
};

typedef int32 CounterType;
typedef uint16_t FeedNameLenType;
typedef uint16_t FetchNameLenType;
typedef uint16_t GraphHandleLenType;
typedef uint64_t StepIdType;

struct StarRunGraphRequest {
  std::string graph_handle_;
  StepIdType step_id_;
  CallOptions opts_;
  std::vector<std::string> feed_names_;
  std::vector<Tensor> feed_tensors_;
  std::vector<std::string> fetch_names_;
  std::vector<bool> is_dead_;
  std::vector<DataType> data_type_;
  int ps_graph_count_;
  // Tracing: should tracing recv wait time
  bool should_tracing_;
  // Tracing: op span context
  std::string op_span_context_;
  // Tracing: the device which recv op run on
  std::string op_device_name_;

  void EncodeRequest(StarBuf* buf);
  void DecodeRequest(const char* buf, size_t len);
};

struct StarRunGraphResponse {
  DeviceBase* device_;
  std::vector<std::string> fetch_names_;
  std::vector<Tensor> fetch_tensors_;
  std::vector<bool> is_dead_;
  std::vector<DataType> data_type_;
};

}  // namespace tensorflow

#endif // TENSORFLOW_CONTRIB_STAR_STAR_TENSOR_CODING_H_
