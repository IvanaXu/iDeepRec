
#
echo
apt-get install libprotobuf-dev protobuf-compiler -y

#
echo
g++ main.cpp -o main && ./main
