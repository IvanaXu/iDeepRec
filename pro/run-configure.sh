# run.sh

#
echo 
echo "> Run"

#
echo 
echo ">> ~protobuf"
cd /pro/test
# apt-get install libprotobuf-dev protobuf-compiler -y
apt-get install cmake -y
cp protobuf-cpp-3.21.9.zip /
cd /
unzip protobuf-cpp-3.21.9.zip
cd protobuf-3.21.9
cmake .
cmake --install .
cmake --build .
protoc --version

g++ main.cpp -o main && ./main

echo
cd /pro/DeepRec/tensorflow/core/framework
ls -l dataset_options* model*
protoc dataset_options.proto --cpp_out=.
protoc model.proto --cpp_out=.
echo && ls -l dataset_options* model*

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
bazel build -c opt --config=opt  --config=mkl_threadpool --define build_with_mkl_dnn_v1_only=true //tensorflow/tools/pip_package:build_pip_package

echo
echo ">> STEP@4"
./bazel-bin/tensorflow/tools/pip_package/build_pip_package /pkg/tensorflow_pkg

# echo
# echo ">> STEP@5"
# pip uninstall tensorflow -y
# pip install /pkg/tensorflow_pkg/tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl

# echo 
# echo ">> STEP@6"
# cd /pro/DeepRec/tianchi
# python run_models.py

#
echo "> Run"

#32 x0.1
