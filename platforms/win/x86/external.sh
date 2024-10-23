#!/bin/bash

set -e

LIBSERIALPORT_SHA=fd20b0fc5a34cd7f776e4af6c763f59041de223b
HIDAPI_SHA=d0732cda906ad07b7e1ef93f1919035643620435

echo "Building libraries..."
echo "  LIBSERIALPORT_SHA: ${LIBSERIALPORT_SHA}"
echo "  HIDAPI_SHA: ${HIDAPI_SHA}"
echo ""

if [ -z "${BUILD_TYPE}" ]; then
   BUILD_TYPE="Release"
fi

echo "Build type: ${BUILD_TYPE}"
echo ""

rm -rf external
mkdir external
cd external

#
# build libserialport and copy to platform/arch
#

curl -sL https://github.com/sigrokproject/libserialport/archive/${LIBSERIALPORT_SHA}.zip -o libserialport.zip
unzip libserialport.zip
cd libserialport-$LIBSERIALPORT_SHA
cp libserialport.h ../../third-party/include
msbuild.exe libserialport.sln -p:Configuration=Release -p:Platform=x86
cp Release/*.lib ../../third-party/build-libs/win/x86
cp Release/*.dll ../../third-party/runtime-libs/win/x86
cd ..

#
# build hdiapi and copy to platform/arch
#

curl -sL https://github.com/libusb/hidapi/archive/${HIDAPI_SHA}.zip -o hidapi.zip
unzip hidapi.zip
cd hidapi-$HIDAPI_SHA
cmake -G "Visual Studio 17 2022" -A Win32 -B build
cmake --build build --config ${BUILD_TYPE}
cp -r hidapi ../../third-party/include/
cp build/src/windows/${BUILD_TYPE}/*.lib ../../third-party/build-libs/win/x86/
cp build/src/windows/${BUILD_TYPE}/*.dll ../../third-party/runtime-libs/win/x86/
cd ..
