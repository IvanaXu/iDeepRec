regurl=ideeprec
ver=v1

docker rmi $regurl:$ver
docker build -t $regurl:$ver .

docker run --name testi $regurl:$ver sh 01.run.sh
docker rm testi
