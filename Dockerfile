FROM ubuntu:20.04
RUN \
  apt-get update && \
  apt-get upgrade -y && \
  apt-get install -y build-essential file git
ADD . /opt/chibicc
WORKDIR /opt/chibicc
