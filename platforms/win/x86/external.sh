#!/bin/bash

set -e

source ./platforms/config.sh

echo "Building libraries..."
echo "  CARGS_SHA: ${CARGS_SHA}"
echo "  LIBSERIALPORT_SHA: ${LIBSERIALPORT_SHA}"
echo "  SOCKPP_SHA: ${SOCKPP_SHA}"
echo "  HIDAPI_SHA: ${HIDAPI_SHA}"
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
   -G "Visual Studio 17 2022" \
   -DBUILD_SHARED_LIBS=ON \
   -A Win32 \
   -B build
cmake --build build --config ${BUILD_TYPE}
cp include/cargs.h ../../third-party/include/
cp build/${BUILD_TYPE}/cargs.lib ../../third-party/build-libs/win/x86/
cp build/${BUILD_TYPE}/cargs.dll ../../third-party/runtime-libs/win/x86/
cd ..

#
# build libserialport and copy to platform/arch
#

curl -sL https://github.com/sigrokproject/libserialport/archive/${LIBSERIALPORT_SHA}.tar.gz -o libserialport-${LIBSERIALPORT_SHA}.tar.gz
tar xzf libserialport-${LIBSERIALPORT_SHA}.tar.gz
mv libserialport-${LIBSERIALPORT_SHA} libserialport
cd libserialport
cp libserialport.h ../../third-party/include
msbuild.exe libserialport.sln \
   -p:Platform=x86 \
   -p:PlatformToolset=v143 \
   -p:Configuration=Release
cp Release/libserialport.lib ../../third-party/build-libs/win/x86
cp Release/libserialport.dll ../../third-party/runtime-libs/win/x86
cd ..

#
# build sockpp and copy to external
#

curl -sL https://github.com/fpagliughi/sockpp/archive/${SOCKPP_SHA}.tar.gz -o sockpp-${SOCKPP_SHA}.tar.gz
tar xzf sockpp-${SOCKPP_SHA}.tar.gz
mv sockpp-${SOCKPP_SHA} sockpp
cd sockpp
cmake \
   -G "Visual Studio 17 2022" \
   -A Win32 \
   -B build
cmake --build build --config ${BUILD_TYPE}
cp -r include/sockpp ../../third-party/include/
cp build/${BUILD_TYPE}/sockpp.lib ../../third-party/build-libs/win/x86/
cp build/${BUILD_TYPE}/sockpp.dll ../../third-party/runtime-libs/win/x86/
cd ..

#
# build hdiapi and copy to platform/arch
#

curl -sL https://github.com/libusb/hidapi/archive/${HIDAPI_SHA}.tar.gz -o hidapi-${HIDAPI_SHA}.tar.gz
tar xzf hidapi-${HIDAPI_SHA}.tar.gz
mv hidapi-${HIDAPI_SHA} hidapi
cd hidapi
cmake \
   -G "Visual Studio 17 2022" \
   -A Win32 \
   -B build
cmake --build build --config ${BUILD_TYPE}
cp -r hidapi ../../third-party/include/
cp build/src/windows/${BUILD_TYPE}/hidapi.lib ../../third-party/build-libs/win/x86/
cp build/src/windows/${BUILD_TYPE}/hidapi.dll ../../third-party/runtime-libs/win/x86/
cd ..