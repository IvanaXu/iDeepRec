echo 
echo "> result"
cd pkg/tensorflow_pkg
zip whl_$(date "+%Y%m%d-%H%M%S").zip tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
ls -l

