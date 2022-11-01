
clear
git pull

cd pkg/tensorflow_pkg/
rm -rf tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
wget https://files.pythonhosted.org/packages/8f/6c/23c292d24b7861af55649f72f05faf0c379b75f75e89580d8a64657f77ad/tensorflow-1.15.5-cp36-cp36m-manylinux2010_x86_64.whl
mv tensorflow-1.15.5-cp36-cp36m-manylinux2010_x86_64.whl tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
ls -l

cd ../../
# sh 00.cmd-configure.sh $1 && sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh $1
sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh 1
