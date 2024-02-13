#!/bin/bash

# $1 - directory
# $2 - target deb architecture. Default - current

set -e

sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
  libboost-stacktrace-dev git

mkdir cpptrace
cd cpptrace
git clone https://github.com/jeremy-rifkin/cpptrace.git .
git checkout v0.4.0
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
sudo make install
cd ../..

DIR=$(dirname $0)
# source $DIR/../set-ver-prms.sh "$1" "$2"

SRC_DIR=$(readlink -f $(dirname $0)/../..)


# PACKAGE_SUFFIX=$4
TARGET_ARCH=${2:$(dpkg --print-architecture)}

PARALLEL_PRMS="-j$(nproc)"

mkdir -p build/linux
pushd build/linux

rm -rf *
export LANG=C

GO_PRMS="-DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DBOOST_ENABLE_ASSERT_DEBUG_HANDLER=ON -DBOOST_STACKTRACE_USE_ADDR2LINE=ON \
  $CMAKE_VERSION_PRMS \
  $($DIR/cmake-prm-yaml-cpp.bash $TARGET_ARCH)"

echo "cmake -G \"Unix Makefiles\" $GO_PRMS . $SRC_DIR"
cmake -G "Unix Makefiles" $GO_PRMS . $SRC_DIR
make -k $PARALLEL_PRMS VERBOSE=1 package

popd
