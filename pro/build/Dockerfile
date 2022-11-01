# Base Images
## 从天池基础镜像构建
FROM alideeprec/deeprec-tianchi-bazel-cache:deeprec-cpu-py36-ubuntu18.04

## 把当前文件夹里的文件构建到镜像的根目录下
ADD . /

## 指定默认工作目录为根目录（需要把run.sh和生成的结果文件都放在该文件夹下，提交后才能运行）
WORKDIR /
