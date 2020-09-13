#!/bin/bash
repo='git@github.com:git/git.git'
. test/thirdparty/common
git reset --hard 54e85e7af1ac9e9a92888060d6811ae767fea1bc

$make clean
$make V=1 CC=$chibicc test
