
clear
git pull

cd pkg/tensorflow_pkg/
rm -rf tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
wget https://files.pythonhosted.org/packages/de/f0/96fb2e0412ae9692dbf400e5b04432885f677ad6241c088ccc5fe7724d69/tensorflow-1.14.0-cp36-cp36m-manylinux1_x86_64.whl -o \ 
tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
ls -l

cd ../../
# sh 00.cmd-configure.sh $1 && sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh $1
sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh 1
