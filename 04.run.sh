
clear
git pull

cd pkg/tensorflow_pkg/
rm -rf tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
wget https://files.pythonhosted.org/packages/7a/ce/e76c4e3d2c245f4f20eff1bf9cbcc602109448142881e1f946ba2d7327bb/tensorflow-2.4.0-cp36-cp36m-manylinux2010_x86_64.whl
mv tensorflow-2.4.0-cp36-cp36m-manylinux2010_x86_64.whl tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
ls -l

cd ../../
# sh 00.cmd-configure.sh $1 && sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh $1
sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh 1
