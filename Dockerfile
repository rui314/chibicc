FROM ubuntu:20.04
RUN \
  apt-get update && \
  apt-get upgrade -y && \
  apt-get install -y build-essential
# dependencies for thirdparty cpython
RUN \
  sed -i 's/# deb-src http:\/\/archive.ubuntu.com\/ubuntu\/ focal-updates universe/deb-src http:\/\/archive.ubuntu.com\/ubuntu\/ focal-updates universe/' /etc/apt/sources.list && \
  apt-get update && \
  DEBIAN_FRONTEND="noninteractive" apt-get build-dep -y python3.9
# dependencies for thirdparty git
RUN \
  DEBIAN_FRONTEND="noninteractive" apt-get install -y gettext libcurl4-openssl-dev libexpat-dev libssl-dev tclsh zlib1g-dev
# dependencies for thirdparty libpng
RUN \
  apt-get install -y zlib1g-dev
# dependencies for thirdparty sqlite
RUN \
  DEBIAN_FRONTEND="noninteractive" apt-get install -y tcl-dev tclsh zlib1g-dev
# dependencies for thirdparty tinycc
RUN \
  DEBIAN_FRONTEND="noninteractive" apt-get install -y texinfo git
ADD . /opt/chibicc
WORKDIR /opt/chibicc
