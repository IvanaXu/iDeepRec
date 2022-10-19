echo  
ls -l pkg/tensorflow_pkg/

echo
cd pkg/tensorflow_pkg/
rm -rf tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
unzip $1

echo
cd ../../
ls -l pkg/tensorflow_pkg/
