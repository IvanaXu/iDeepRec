
#
echo && echo
## protobuf
apt-get install libprotobuf-dev protobuf-compiler -y

echo
protoc model.proto --cpp_out=.
## abseil
# git clone https://github.com/abseil/abseil-cpp.git
# cd abseil-cpp
# bazel test --test_tag_filters=-benchmark @com_google_absl//...

#
echo && echo
cd ../
g++ main.cpp -o main && ./main
