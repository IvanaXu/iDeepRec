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
apt-get install libprotobuf-dev protobuf-compiler -y
./configure

# 
echo
echo ">> STEP@3"
bazel build  -c opt --config=opt  --config=mkl_threadpool --define build_with_mkl_dnn_v1_only=true //tensorflow/tools/pip_package:build_pip_package

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
