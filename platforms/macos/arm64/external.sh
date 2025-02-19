#!/bin/bash

set -e

CARGS_SHA=5949a20a926e902931de4a32adaad9f19c76f251
SOCKPP_SHA=e6c4688a576d95f42dd7628cefe68092f6c5cd0f
LIBSERIALPORT_SHA=fd20b0fc5a34cd7f776e4af6c763f59041de223b
HIDAPI_SHA=d0732cda906ad07b7e1ef93f1919035643620435

echo "Building libraries..."
echo "  CARGS_SHA: ${CARGS_SHA}"
echo "  SOCKPP_SHA: ${SOCKPP_SHA}"
echo "  LIBSERIALPORT_SHA: ${LIBSERIALPORT_SHA}"
echo "  HIDAPI_SHA: ${HIDAPI_SHA}"
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
   -DBUILD_SHARED_LIBS=ON \
   -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
   -DCMAKE_OSX_ARCHITECTURES=arm64 \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp include/cargs.h ../../third-party/include/
cp build/libcargs.dylib ../../third-party/runtime-libs/macos/arm64/
cd ..

#
# build sockpp and copy to external
#

curl -sL https://github.com/fpagliughi/sockpp/archive/${SOCKPP_SHA}.tar.gz -o sockpp-${SOCKPP_SHA}.tar.gz
tar xzf sockpp-${SOCKPP_SHA}.tar.gz
mv sockpp-${SOCKPP_SHA} sockpp
cd sockpp
cmake -DSOCKPP_BUILD_SHARED=ON \
   -DSOCKPP_BUILD_STATIC=OFF \
   -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
   -DCMAKE_OSX_ARCHITECTURES=arm64 \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp -r include/sockpp ../../third-party/include/
cp -a build/libsockpp.{dylib,*.dylib} ../../third-party/runtime-libs/macos/arm64/
cd ..

#
# build libserialport and copy to platform/arch
#

curl -sL https://github.com/sigrokproject/libserialport/archive/${LIBSERIALPORT_SHA}.tar.gz -o libserialport-${LIBSERIALPORT_SHA}.tar.gz
tar xzf libserialport-${LIBSERIALPORT_SHA}.tar.gz
mv libserialport-${LIBSERIALPORT_SHA} libserialport
cd libserialport
./autogen.sh
./configure --host=aarch64-apple-darwin CFLAGS="-arch arm64" LDFLAGS="-Wl,-install_name,@rpath/libserialport.dylib"
make -j${NUM_PROCS}
cp libserialport.h ../../third-party/include
cp .libs/*.a ../../third-party/build-libs/macos/arm64
cp -a .libs/libserialport.{dylib,*.dylib} ../../third-party/runtime-libs/macos/arm64
cd ..

#
# build hdiapi and copy to platform/arch
#

curl -sL https://github.com/libusb/hidapi/archive/${HIDAPI_SHA}.tar.gz -o hidapi-${HIDAPI_SHA}.tar.gz
tar xzf hidapi-${HIDAPI_SHA}.tar.gz
mv hidapi-${HIDAPI_SHA} hidapi
cd hidapi
cmake \
   -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
   -DCMAKE_OSX_ARCHITECTURES=arm64 \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp -r hidapi ../../third-party/include/
cp -a build/src/mac/libhidapi.{dylib,*.dylib} ../../third-party/runtime-libs/macos/arm64/
cd ..
