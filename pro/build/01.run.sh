
#
echo && echo
## protobuf
apt-get install libprotobuf-dev protobuf-compiler -y

echo
## abseil
cd abseil
bazel test --test_tag_filters=-benchmark @com_google_absl//...

#
echo && echo
cd ../
g++ main.cpp -o main && ./main
