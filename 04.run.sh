
clear
rm -rf pkg/tensorflow_pkg/tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
ls -l pkg/tensorflow_pkg/

sh 00.cmd-configure.sh $1 && sh 01.cmd-submit.sh && sh 03.cmd-runWHL.sh $1
