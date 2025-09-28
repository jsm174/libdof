#!/bin/bash

set -e

source ./platforms/config.sh

echo "Building libraries..."
echo "  LIBUSB_SHA: ${LIBUSB_SHA}"
echo "  LIBSERIALPORT_SHA: ${LIBSERIALPORT_SHA}"
echo "  HIDAPI_SHA: ${HIDAPI_SHA}"
echo "  LIBFTDI_SHA: ${LIBFTDI_SHA}"
echo ""

NUM_PROCS=$(sysctl -n hw.ncpu)

rm -rf external
mkdir external
cd external

#
# build libusb and copy to third-party
#

curl -sL https://github.com/libusb/libusb/archive/${LIBUSB_SHA}.tar.gz -o libusb-${LIBUSB_SHA}.tar.gz
tar xzf libusb-${LIBUSB_SHA}.tar.gz
mv libusb-${LIBUSB_SHA} libusb
cd libusb
./autogen.sh
./configure \
   --host=x86_64-apple-darwin \
   CFLAGS="-arch x86_64" \
   LDFLAGS="-Wl,-install_name,@rpath/libusb-1.0.dylib"
make -j${NUM_PROCS}
mkdir -p ../../third-party/include/libusb-1.0
cp libusb/libusb.h ../../third-party/include/libusb-1.0
cp -a libusb/.libs/libusb*.dylib ../../third-party/runtime-libs/macos/x64/
cd ..

#
# build libserialport and copy to third-party
#

curl -sL https://github.com/sigrokproject/libserialport/archive/${LIBSERIALPORT_SHA}.tar.gz -o libserialport-${LIBSERIALPORT_SHA}.tar.gz
tar xzf libserialport-${LIBSERIALPORT_SHA}.tar.gz
mv libserialport-${LIBSERIALPORT_SHA} libserialport
cd libserialport
./autogen.sh
./configure --host=x86_64-apple-darwin CFLAGS="-arch x86_64" LDFLAGS="-Wl,-install_name,@rpath/libserialport.dylib"
make -j${NUM_PROCS}
cp libserialport.h ../../third-party/include
cp .libs/*.a ../../third-party/build-libs/macos/x64
cp -a .libs/libserialport.{dylib,*.dylib} ../../third-party/runtime-libs/macos/x64
cd ..

#
# build hdiapi and copy to third-party
#

curl -sL https://github.com/libusb/hidapi/archive/${HIDAPI_SHA}.tar.gz -o hidapi-${HIDAPI_SHA}.tar.gz
tar xzf hidapi-${HIDAPI_SHA}.tar.gz
mv hidapi-${HIDAPI_SHA} hidapi
cd hidapi
cmake \
   -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
   -DCMAKE_OSX_ARCHITECTURES=x86_64 \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp -r hidapi ../../third-party/include/
cp -a build/src/mac/libhidapi.{dylib,*.dylib} ../../third-party/runtime-libs/macos/x64/
cd ..

#
# build libftdi and copy to third-party
#

curl -sL https://github.com/jsm174/libftdi/archive/${LIBFTDI_SHA}.tar.gz -o libftdi-${LIBFTDI_SHA}.tar.gz
tar xzf libftdi-${LIBFTDI_SHA}.tar.gz
mv libftdi-${LIBFTDI_SHA} libftdi
cd libftdi
sed -i.bak 's/cmake_minimum_required(VERSION 2.6 FATAL_ERROR)/cmake_minimum_required(VERSION 3.10)\ncmake_policy(SET CMP0042 NEW)/' CMakeLists.txt
cmake \
   -DFTDI_EEPROM=OFF \
   -DEXAMPLES=OFF \
   -DSTATICLIBS=OFF \
   -DLIBUSB_INCLUDE_DIR=../libusb/libusb \
   -DLIBUSB_LIBRARIES=$(pwd)/../libusb/libusb/.libs/libusb-1.0.dylib \
   -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
   -DCMAKE_OSX_ARCHITECTURES=x86_64 \
   -DCMAKE_INSTALL_NAME_DIR=@rpath \
   -DCMAKE_INSTALL_RPATH=@loader_path \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
mkdir -p ../../third-party/include/libftdi1
cp src/ftdi.h ../../third-party/include/libftdi1
cp -a build/src/libftdi*.dylib ../../third-party/runtime-libs/macos/x64/
cd ..