#!/bin/bash

set -e

CARGS_SHA=5949a20a926e902931de4a32adaad9f19c76f251
SOCKPP_SHA=e6c4688a576d95f42dd7628cefe68092f6c5cd0f

echo "Building libraries..."
echo "  CARGS_SHA: ${CARGS_SHA}"
echo "  SOCKPP_SHA: ${SOCKPP_SHA}"
echo ""

if [ -z "${BUILD_TYPE}" ]; then
   BUILD_TYPE="Release"
fi

NUM_PROCS=$(sysctl -n hw.ncpu)

echo "Build type: ${BUILD_TYPE}"
echo "Procs: ${NUM_PROCS}"
echo ""

rm -rf external
mkdir external
cd external

#
# build cargs and copy to external
#

curl -sL https://github.com/likle/cargs/archive/${CARGS_SHA}.tar.gz -o cargs-${CARGS_SHA}.tar.gz
tar xzf cargs-${CARGS_SHA}.tar.gz
mv cargs-${CARGS_SHA} cargs
cd cargs
cmake \
   -DCMAKE_SYSTEM_NAME=iOS \
   -DCMAKE_OSX_ARCHITECTURES=arm64 \
   -DCMAKE_OSX_DEPLOYMENT_TARGET=17.0 \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp include/cargs.h ../../third-party/include/
cp build/libcargs.a ../../third-party/build-libs/ios/arm64/
cd ..

#
# build sockpp and copy to external
#

curl -sL https://github.com/fpagliughi/sockpp/archive/${SOCKPP_SHA}.tar.gz -o sockpp-${SOCKPP_SHA}.tar.gz
tar xzf sockpp-${SOCKPP_SHA}.tar.gz
mv sockpp-${SOCKPP_SHA} sockpp
cd sockpp
cmake \
   -DSOCKPP_BUILD_SHARED=OFF \
   -DSOCKPP_BUILD_STATIC=ON \
   -DCMAKE_SYSTEM_NAME=iOS \
   -DCMAKE_OSX_ARCHITECTURES=arm64 \
   -DCMAKE_OSX_DEPLOYMENT_TARGET=17.0 \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp -r include/sockpp ../../third-party/include/
cp build/libsockpp.a ../../third-party/build-libs/ios/arm64/
cd ..
