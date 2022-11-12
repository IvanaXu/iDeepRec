This maintenance release of Intel® Optimizations for TensorFlow* 1.15 UP3 Release is
based on the TensorFlow v1.15.0up3 tag (https://github.com/Intel-tensorflow/tensorflow.git) 
as built with support for oneAPI Deep Neural Network Library (oneDNN v2.2.4).

Compared with [public TensorFlow 1.15.0 release](https://github.com/tensorflow/tensorflow/releases/tag/v1.15.0), it contains oneDNN 2.2.4 update
and important MKL optimization PRs backport from public TensorFlow 2.4.0 release, 
including Eigen threadpool feature, Bfloat16 data type support and important optimizations.
Learn more details about the PRs backport list from [TensorFlow 1.15 UP3](https://github.com/Intel-tensorflow/tensorflow/releases/tag/v1.15.0up3) release notes.

## Source Build
Installation instructions:
1. Ensure numpy, keras-applications, keras-preprocessing, pip, six, wheel, mock 
packages are installed in the Python environment where TensorFlow is being built
and installed.
2. Clone the TensorFlow source code and checkout this release branch for source 
build preparation.
```
$ git clone https://github.com/Intel-tensorflow/tensorflow.git
$ git checkout v1.15.0up3
```
3. Run "./configure" from the TensorFlow source directory.
4. Execute the following commands to create a pip package that can be used to 
install the optimized TensorFlow build.

PATH can be changed to point to a specific version of GCC compiler:
```
export PATH=/PATH//bin:$PATH
```
LD_LIBRARY_PATH can also be to new:
```
export LD_LIBRARY_PATH=/PATH//lib64:$LD_LIBRARY_PATH
```
Set the compiler flags support by the GCC on your machine to build TensorFlow with oneDNN.
```
bazel build --cxxopt=-D_GLIBCXX_USE_CXX11_ABI=0 --config=opt  --copt=-O3 --copt=-Wformat 
            --copt=-Wformat-security --copt=-fstack-protector --copt=-fPIC --copt=-fpic 
            --linkopt=-znoexecstack --linkopt=-zrelro --linkopt=-znow 
            --linkopt=-fstack-protector --config=mkl --copt=-march=native 
            //tensorflow/tools/pip_package:build_pip_package
```
5. Install the optimized TensorFlow wheel
```
bazel-bin/tensorflow/tools/pip_package/build_pip_package ~/path_to_save_wheel
```
```
pip install --upgrade --user ~/path_to_save_wheel/<wheel_name>.whl
```

## Binaries Install

This maintenance release only provide the wheel binaries for download and installation in Linux.

Run the below instruction to install the wheel into an existing Python* installation. Python 
versions supported are 3.5, 3.6, and 3.7. The commands to install the current release for different
Python versions are as below:

```
$ pip install https://storage.googleapis.com/intel-optimized-tensorflow/intel_tensorflow-1.15.0up3-cp35-cp35m-manylinux2010_x86_64.whl
```
```
$ pip install https://storage.googleapis.com/intel-optimized-tensorflow/intel_tensorflow-1.15.0up3-cp36-cp36m-manylinux_2_12_x86_64.manylinux2010_x86_64.whl
```
```
$ pip install https://storage.googleapis.com/intel-optimized-tensorflow/intel_tensorflow-1.15.0up3-cp37-cp37m-manylinux_2_12_x86_64.manylinux2010_x86_64.whl
```

Learn more about the [TensorFlow community](https://www.tensorflow.org/community) and how to [contribute](https://www.tensorflow.org/community/contribute).

## License

[Apache License 2.0](LICENSE)

