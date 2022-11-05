# rcmd.sh

# devp=/root/data/code/iDeepRec
mainp=/root/data/code/iDeepRec

# 
echo "> Git"
cd $mainp
git pull

# 
echo 
echo "> Start"
cd $mainp
# rm -rf pro/DeepRec
# cp -r $devp pro/DeepRec
tree -L 2



# docker
echo
echo "> Docker"
docker stop tianchi_test
docker rm tianchi_test


# docker run -ti \
#     --name=tianchi_test \
#     --net=host \
#     -v $mainp/pro:/pro \
#     -v $mainp/pkg:/pkg \
#     -v $mainp/cache:/root/.cache/ \
#     alideeprec/deeprec-tianchi:deeprec-cpu-py36-ubuntu18.04 \
#     bash

# docker run -ti \
#     --name=tianchi_test \
#     --net=host \
#     -v $mainp/pro:/pro \
#     -v $mainp/pkg:/pkg \
#     alideeprec/deeprec-tianchi:deeprec-cpu-py36-ubuntu18.04 \
#     bash /pro/pro.sh

# docker run -ti \
#     --name=tianchi_test \
#     --net=host \
#     -v $mainp/pro:/pro \
#     -v $mainp/pkg:/pkg \
#     -v $mainp/cache:/root/.cache/ \
#     alideeprec/deeprec-tianchi:deeprec-cpu-py36-ubuntu18.04 \
#     bash /pro/pro.sh

docker run -ti \
    --name=tianchi_test \
    --net=host \
    -v $mainp/pro:/pro \
    -v $mainp/pkg:/pkg \
    alideeprec/deeprec-build:deeprec-dev-cpu-py36-ubuntu18.04 \
    bash /pro/pro-configure.sh $1

echo $(date "+%Y-%m-%d %H:%M:%S")


# zip
echo 
echo "> result"
cd pkg/tensorflow_pkg
# zip whl_$(date "+%Y%m%d-%H%M%S").zip tensorflow-1.15.5+deeprec2206-cp36-cp36m-linux_x86_64.whl
ls -l


#
echo 
echo "> END"

