
clear
git pull

cd pkg/tensorflow_pkg/
rm -rf tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
wget https://files.pythonhosted.org/packages/ef/17/01bdb5f1e47c1a5b39c58f138189df98e07feccf86935a834505f333ac6e/tensorflow-2.5.0-cp36-cp36m-manylinux2010_x86_64.whl
mv tensorflow-2.5.0-cp36-cp36m-manylinux2010_x86_64.whl tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
ls -l

cd ../../
# sh 00.cmd-configure.sh $1 && sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh $1
sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh 1
