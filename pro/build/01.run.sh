
#
echo && echo
## protobuf
apt-get install libprotobuf-dev protobuf-compiler -y

echo
## abseil
git clone https://github.com/abseil/abseil-cpp.git && cd abseil-cpp/
bazel build //absl/...

#
echo && echo
g++ main.cpp -o main && ./main
