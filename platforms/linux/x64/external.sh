#!/bin/bash

set -e

source ./platforms/config.sh

echo "Building libraries..."
echo "  LIBUSB_SHA: ${LIBUSB_SHA}"
echo "  LIBSERIALPORT_SHA: ${LIBSERIALPORT_SHA}"
echo "  HIDAPI_SHA: ${HIDAPI_SHA}"
echo "  LIBFTDI_SHA: ${LIBFTDI_SHA}"
echo ""

NUM_PROCS=$(nproc)

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
   --enable-shared
make -j${NUM_PROCS}
cp libusb/libusb.h ../../third-party/include/
cp -a libusb/.libs/libusb-1.0.{so,so.*} ../../third-party/runtime-libs/linux/x64/
cd ..

#
# build libserialport and copy to third-party
#

curl -sL https://github.com/sigrokproject/libserialport/archive/${LIBSERIALPORT_SHA}.tar.gz -o libserialport-${LIBSERIALPORT_SHA}.tar.gz
tar xzf libserialport-${LIBSERIALPORT_SHA}.tar.gz
mv libserialport-${LIBSERIALPORT_SHA} libserialport
cd libserialport
cp libserialport.h ../../third-party/include
./autogen.sh
./configure
make -j${NUM_PROCS}
cp .libs/libserialport.a ../../third-party/build-libs/linux/x64
cp -a .libs/libserialport.{so,so.*} ../../third-party/runtime-libs/linux/x64
cd ..

#
# build hdiapi and copy to third-party
#

curl -sL https://github.com/libusb/hidapi/archive/${HIDAPI_SHA}.tar.gz -o hidapi-${HIDAPI_SHA}.tar.gz
tar xzf hidapi-${HIDAPI_SHA}.tar.gz
mv hidapi-${HIDAPI_SHA} hidapi
cd hidapi
cmake \
   -DHIDAPI_WITH_LIBUSB=OFF \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp -r hidapi ../../third-party/include/
cp -a build/src/linux/libhidapi-hidraw.{so,so.*} ../../third-party/runtime-libs/linux/x64
cd ..

#
# build libftdi and copy to third-party
#

curl -sL "http://developer.intra2net.com/git/?p=libftdi;a=snapshot;h=${LIBFTDI_SHA};sf=tgz" -o libftdi-${LIBFTDI_SHA}.tar.gz
tar xzf libftdi-${LIBFTDI_SHA}.tar.gz
mv libftdi-${LIBFTDI_SHA:0:7} libftdi
cd libftdi
cmake \
   -DFTDI_EEPROM=OFF \
   -DEXAMPLES=OFF \
   -DSTATICLIBS=OFF \
   -DLIBUSB_INCLUDE_DIR=../libusb/libusb \
   -DLIBUSB_LIBRARIES=$(pwd)/../libusb/libusb/.libs/libusb-1.0.so \
   -DCMAKE_INSTALL_RPATH='$$ORIGIN' \
   -DCMAKE_BUILD_WITH_INSTALL_RPATH=TRUE \
   -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
   -B build
cmake --build build -- -j${NUM_PROCS}
cp src/ftdi.h ../../third-party/include/
cp -a build/src/libftdi1.{so,so.*} ../../third-party/runtime-libs/linux/x64
cd ..