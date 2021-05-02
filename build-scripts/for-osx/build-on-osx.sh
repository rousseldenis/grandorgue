#!/bin/bash

# $1 - Version
# $2 - Build version
# $3 - Go source Dir. If not set then relative to the script dir

set -e

source $(dirname $0)/../set-ver-prms.sh "$1" "$2"


if [[ -n "$3" ]]; then
	SRC_DIR=$3
else
	SRC_DIR=$(dirname $0)/../..
fi

PARALLEL_PRMS="-j4"

mkdir -p build-for/osx
pushd build-for/osx

rm -rf *
export LANG=C

OS_PRMS="-DDOCBOOK_DIR=/usr/local/opt/docbook-xsl/docbook-xsl"
GO_PRMS="-DCMAKE_BUILD_TYPE=Debug $CMAKE_VERSION_PRMS"
cmake -G "Unix Makefiles" $OS_PRMS $GO_PRMS . $SRC_DIR
make -k $PARALLEL_PRMS VERBOSE=1 package

popd

