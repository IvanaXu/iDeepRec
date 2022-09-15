
![DeepRec Logo](https://github.com/alibaba/DeepRec/blob/main/docs/deeprec_logo.png)

--------------------------------------------------------------------------------

## **Introduction**
DeepRec is a recommendation engine based on [TensorFlow 1.15](https://www.tensorflow.org/), [Intel-TensorFlow](https://github.com/Intel-tensorflow/tensorflow) and [NVIDIA-TensorFlow](https://github.com/NVIDIA/tensorflow).


### **Background**
Sparse model is a type of deep learning model that accounts for a relatively high proportion of discrete feature calculation logic in the model structure. Discrete features are usually expressed as non-numeric features that cannot be directly processed by algorithms such as id, tag, text, and phrases. They are widely used in high-value businesses such as search, advertising, and recommendation.


DeepRec has been deeply cultivated since 2016, which supports core businesses such as Taobao Search, recommendation and advertising. It precipitates a list of features on basic frameworks and has excellent performance in sparse models training. Facing a wide variety of external needs and the environment of deep learning framework embracing open source, DeepeRec open source is conducive to establishing standardized interfaces, cultivating user habits, greatly reducing the cost of external customers working on cloud and establishing the brand value.

### **Key Features**
DeepRec has super large-scale distributed training capability, supporting model training of trillion samples and 100 billion Embedding Processing. For sparse model scenarios, in-depth performance optimization has been conducted across CPU and GPU platform. It contains 3 kinds of features to improve usability and performance for super-scale scenarios. 
#### **Sparse Functions**
 - Embedding Variable.
 - Dynamic Dimension Embedding Variable.
 - Adaptive Embedding Variable.
 - Multiple Hash Embedding Variable.
 - Multi-tier Hybrid Embedding Storage
 #### **Performance Optimization**
 - Distributed Training Framework Optimization, such as grpc+seastar, FuseRecv, StarServer, HybridBackend etc.
 - Runtime Optimization, such as CPU memory allocator (PRMalloc), GPU memory allocator, Cost based and critical path first Executor etc.
 - Operator level optimization, such as BF16 mixed precision  optimization, sparse operator optimization and EmbeddingVariable on PMEM and GPU, new hardware feature enabling, etc.
 - Graph level optimization, such as AutoGraphFusion, SmartStage, AutoPipeline, StrutureFeature, MicroBatch etc.
 - Compilation optimization, support BladeDISC, XLA etc.
#### **Deploy and Serving**
 - Incremental model loading and exporting.
 - Super-scale sparse model distributed serving.
 - Multi-tier hybrid storage and multi backend supported.
 - Online deep learning with low latency.
 - High performance processor with SessionGroup supported.


***
## **Installation**


### **Prepare for installation**

**CPU Platform**

```
registry.cn-shanghai.aliyuncs.com/pai-dlc-share/deeprec-developer:deeprec-dev-cpu-py36-ubuntu18.04
```

Docker Hub repository

``````
alideeprec/deeprec-build:deeprec-dev-cpu-py36-ubuntu18.04
``````

**GPU Platform**

```
registry.cn-shanghai.aliyuncs.com/pai-dlc-share/deeprec-developer:deeprec-dev-gpu-py36-cu110-ubuntu18.04
```

Docker Hub repository

```
alideeprec/deeprec-build:deeprec-dev-gpu-py36-cu110-ubuntu18.04
```

### **How to Build**

Configure
```
$ ./configure
```
Compile for CPU and GPU defaultly
```
$ bazel build -c opt --config=opt //tensorflow/tools/pip_package:build_pip_package
```
Compile for CPU and GPU: ABI=0
```
$ bazel build --cxxopt="-D_GLIBCXX_USE_CXX11_ABI=0" --host_cxxopt="-D_GLIBCXX_USE_CXX11_ABI=0" -c opt --config=opt //tensorflow/tools/pip_package:build_pip_package
```
Compile for CPU optimization: oneDNN + Unified Eigen Thread pool
```
$ bazel build -c opt --config=opt --config=mkl_threadpool //tensorflow/tools/pip_package:build_pip_package
```
Compile for CPU optimization and ABI=0
```
$ bazel build --cxxopt="-D_GLIBCXX_USE_CXX11_ABI=0" --host_cxxopt="-D_GLIBCXX_USE_CXX11_ABI=0" -c opt --config=opt --config=mkl_threadpool //tensorflow/tools/pip_package:build_pip_package
```
### **Create whl package** 
```
$ ./bazel-bin/tensorflow/tools/pip_package/build_pip_package /tmp/tensorflow_pkg
```
### **Install whl package**
```
$ pip3 install /tmp/tensorflow_pkg/tensorflow-1.15.5+${version}-cp36-cp36m-linux_x86_64.whl
```

### **Latest Release Images**
#### Image for GPU CUDA11.0
```
registry.cn-shanghai.aliyuncs.com/pai-dlc-share/deeprec-training:deeprec2204u1-gpu-py36-cu110-ubuntu18.04
```
Docker Hub repository

```
alideeprec/deeprec-release:deeprec2204u1-gpu-py36-cu110-ubuntu18.04
```

#### Image for CPU

```
registry.cn-shanghai.aliyuncs.com/pai-dlc-share/deeprec-training:deeprec2204u1-cpu-py36-ubuntu18.04
```
Docker Hub repository
```
alideeprec/deeprec-release:deeprec2204u1-cpu-py36-ubuntu18.04
```

***
## Continuous Build Status

### Official Build

| Build Type    | Status                                                       |
| ------------- | ------------------------------------------------------------ |
| **Linux CPU** | ![CPU Build](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-build-wheel.yaml/badge.svg) |
| **Linux GPU** | ![GPU Build](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-build-wheel.yaml/badge.svg) |
| **Linux CPU Serving** | ![CPU Serving Build](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-build-serving.yaml/badge.svg) |

### Official Unit Tests

| Unit Test Type | Status |
| -------------- | ------ |
| **Linux CPU C** | ![CPU C Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-c-unit-test.yaml/badge.svg) |
| **Linux CPU CC** | ![CPU CC Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-cc-unit-test.yaml/badge.svg) |
| **Linux CPU Contrib** | ![CPU Contrib Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-contrib-unit-test.yaml/badge.svg) |
| **Linux CPU Core** | ![CPU Core Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-core-unit-test.yaml/badge.svg) |
| **Linux CPU Examples** | ![CPU Examples Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-examples-unit-test.yaml/badge.svg) |
| **Linux CPU Java** | ![CPU Java Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-java-unit-test.yaml/badge.svg) |
| **Linux CPU JS** | ![CPU JS Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-js-unit-test.yaml/badge.svg) |
| **Linux CPU Python** | ![CPU Python Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-python-unit-test.yaml/badge.svg) |
| **Linux CPU Stream Executor** | ![CPU Stream Executor Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-stream_executor-unit-test.yaml/badge.svg) |
| **Linux GPU C** | ![GPU C Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-c-unit-test.yaml/badge.svg) |
| **Linux GPU CC** | ![GPU CC Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-cc-unit-test.yaml/badge.svg) |
| **Linux GPU Contrib** | ![GPU Contrib Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-contrib-unit-test.yaml/badge.svg) |
| **Linux GPU Core** | ![GPU Core Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-core-unit-test.yaml/badge.svg) |
| **Linux GPU Examples** | ![GPU Examples Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-examples-unit-test.yaml/badge.svg) |
| **Linux GPU Java** | ![GPU Java Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-java-unit-test.yaml/badge.svg) |
| **Linux GPU JS** | ![GPU JS Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-js-unit-test.yaml/badge.svg) |
| **Linux GPU Python** | ![GPU Python Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-python-unit-test.yaml/badge.svg) |
| **Linux GPU Stream Executor** | ![GPU Stream Executor Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cuda11.2-cibuild-stream_executor-unit-test.yaml/badge.svg) |
| **Linux CPU Serving UT** | ![CPU Serving Unit Tests](https://github.com/alibaba/DeepRec/actions/workflows/ubuntu18.04-py3.6-cibuild-serving-unit-test.yaml/badge.svg) |

## **User Document (Chinese)**

[https://deeprec.rtfd.io](https://deeprec.rtfd.io)

## **Contact Us**

Join the Official Discussion Group on DingTalk

<img src="docs/README/deeprec_dingtalk.png" width="200">

## **License**

[Apache License 2.0](LICENSE)

## iDeepRec
```
├── cache
│   ├── bazel
│   └── pip
├── dev
│   └── iDeepRec
├── pkg
│   └── tensorflow_pkg
├── pro
│   ├── DeepRec
│   ├── pro_log
│   ├── pro.sh
│   └── run.sh
└── rcmd.sh
```
* cache, cache for configure
* dev, git pull this code branch
```shell
git pull
```
* pkg, result pkg after run
* pro, copy branch for configure&run
```shell
# pro.sh
sh /pro/run.sh|tee /pro/pro_log/run_$(date "+%Y%m%d-%H%M%S").log
```
```shell
# run.sh

#
echo 
echo "> Run"

#
echo
echo ">> STEP@1"
cd /pro/DeepRec
ls -l

# 
echo
echo ">> STEP@2"
./configure

# 
echo
echo ">> STEP@3"
bazel build  -c opt --config=opt  --config=mkl_threadpool --define build_with_mkl_dnn_v1_only=true //tensorflow/tools/pip_package:build_pip_package

echo
echo ">> STEP@4"
./bazel-bin/tensorflow/tools/pip_package/build_pip_package /pkg/tensorflow_pkg

echo
echo ">> STEP@5"
pip uninstall tensorflow -y
pip install /pkg/tensorflow_pkg/tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl

echo 
echo ">> STEP@6"
cd /pro/DeepRec/tianchi
python run_models.py

#
echo "> Run"

```
* rcmd.sh, main shell
```shell
# rcmd.sh

clear

devp=/root/data/code/iDeepRec
mainp=/root/data/code/DeepRec

# 
echo "> Git"
cd $devp
git pull

# 
echo 
echo "> Start"
cd $mainp
rm -rf pro/DeepRec
cp -r $devp pro/DeepRec
tree -L 2



# docker
echo
echo "> Docker"
docker stop tianchi_test
docker rm tianchi_test


# docker run -ti \
#     --name=tianchi_test \
#     --net=host \
#     -v $mainp/pro:/pro \
#     -v $mainp/pkg:/pkg \
#     -v $mainp/cache:/root/.cache/ \
#     alideeprec/deeprec-tianchi:deeprec-cpu-py36-ubuntu18.04 \
#     bash

# docker run -ti \
#     --name=tianchi_test \
#     --net=host \
#     -v $mainp/pro:/pro \
#     -v $mainp/pkg:/pkg \
#     alideeprec/deeprec-tianchi:deeprec-cpu-py36-ubuntu18.04 \
#     bash /pro/pro.sh

# docker run -ti \
#     --name=tianchi_test \
#     --net=host \
#     -v $mainp/pro:/pro \
#     -v $mainp/pkg:/pkg \
#     -v $mainp/cache:/root/.cache/ \
#     alideeprec/deeprec-tianchi:deeprec-cpu-py36-ubuntu18.04 \
#     bash /pro/pro.sh

docker run -ti \
    --name=tianchi_test \
    --net=host \
    -v $mainp/pro:/pro \
    -v $mainp/pkg:/pkg \
    alideeprec/deeprec-tianchi-bazel-cache:deeprec-cpu-py36-ubuntu18.04 \
    bash /pro/pro.sh

echo $(date "+%Y-%m-%d %H:%M:%S")


# zip
echo 
echo "> result"
cd pkg/tensorflow_pkg
# zip whl_$(date "+%Y%m%d-%H%M%S").zip tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
ls -l


#
echo 
echo "> END"

```
