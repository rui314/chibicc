make="make -j$(nproc)"
chibicc=`pwd`/chibicc

dir=$(basename -s .git $repo)

set -e -x

mkdir -p thirdparty
cd thirdparty
[ -d $dir ] || git clone $repo
cd $dir
