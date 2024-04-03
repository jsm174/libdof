#!/bin/bash

set -e

CARGS_SHA=5949a20a926e902931de4a32adaad9f19c76f251
SOCKPP_SHA=e6c4688a576d95f42dd7628cefe68092f6c5cd0f
LIBSERIALPORT_SHA=fd20b0fc5a34cd7f776e4af6c763f59041de223b
HIDAPI_SHA=d0732cda906ad07b7e1ef93f1919035643620435

NUM_PROCS=$(nproc)

echo "Building libraries..."
echo "  CARGS_SHA: ${CARGS_SHA}"
echo "  SOCKPP_SHA: ${SOCKPP_SHA}"
echo "  LIBSERIALPORT_SHA: ${LIBSERIALPORT_SHA}"
echo "  HIDAPI_SHA: ${HIDAPI_SHA}"
echo ""

if [ -z "${BUILD_TYPE}" ]; then
   BUILD_TYPE="Release"
fi

echo "Build type: ${BUILD_TYPE}"
echo "Procs: ${NUM_PROCS}"
echo ""

rm -rf external
mkdir external
cd external

#
# build cargs and copy to external
#

curl -sL https://github.com/likle/cargs/archive/${CARGS_SHA}.zip -o cargs.zip
unzip cargs.zip
cd cargs-${CARGS_SHA}
cmake \
   -DBUILD_SHARED_LIBS=ON \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp include/cargs.h ../../third-party/include/
cp -a build/*.so ../../third-party/runtime-libs/linux/x64/
cd ..

#
# build sockpp and copy to external
#

curl -sL https://github.com/fpagliughi/sockpp/archive/${SOCKPP_SHA}.zip -o sockpp.zip
unzip sockpp.zip
cd sockpp-$SOCKPP_SHA
cmake \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp -r include/sockpp ../../third-party/include/
cp -a build/libsockpp.{so,so.*} ../../third-party/runtime-libs/linux/x64/
cd ..

#
# build libserialport and copy to platform/arch
#

curl -sL https://github.com/sigrokproject/libserialport/archive/${LIBSERIALPORT_SHA}.zip -o libserialport.zip
unzip libserialport.zip
cd libserialport-$LIBSERIALPORT_SHA
cp libserialport.h ../../third-party/include
./autogen.sh
./configure
make -j${NUM_PROCS}
cp .libs/*.a ../../third-party/build-libs/linux/x64
cp -a .libs/*.{so,so.*} ../../third-party/runtime-libs/linux/x64
cd ..

#
# build hdiapi and copy to platform/arch
#

curl -sL https://github.com/libusb/hidapi/archive/${HIDAPI_SHA}.zip -o hidapi.zip
unzip hidapi.zip
cd hidapi-$HIDAPI_SHA
cmake \
   -DHIDAPI_WITH_LIBUSB=OFF \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp hidapi/hidapi.h ../../third-party/include/
cp -a build/src/linux/*.{so,so.*} ../../third-party/runtime-libs/linux/x64
cd ..