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
bazel build --cxxopt=-D_GLIBCXX_USE_CXX11_ABI=0 --config=opt --copt=-O3 --copt=-Wformat --copt=-Wformat-security --copt=-fstack-protector --copt=-fPIC --copt=-fpic --linkopt=-znoexecstack --linkopt=-zrelro --linkopt=-znow --linkopt=-fstack-protector --config=mkl --copt=-march=native --define=tensorflow_mkldnn_contraction_kernel=1 //tensorflow/tools/pip_package:build_pip_package

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
