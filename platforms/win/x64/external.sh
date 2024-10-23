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
patch libserialport.vcxproj < ../../platforms/win/x64/libserialport/001.patch
msbuild.exe libserialport.sln -p:Configuration=Release -p:Platform=x64
cp x64/Release/*.lib ../../third-party/build-libs/win/x64
cp x64/Release/*.dll ../../third-party/runtime-libs/win/x64
cd ..

#
# build hdiapi and copy to platform/arch
#

curl -sL https://github.com/libusb/hidapi/archive/${HIDAPI_SHA}.zip -o hidapi.zip
unzip hidapi.zip
cd hidapi-$HIDAPI_SHA
patch -p1 < ../../platforms/win/x64/hidapi/001.patch
cmake -G "Visual Studio 17 2022" -B build
cmake --build build --config ${BUILD_TYPE}
cp -r hidapi ../../third-party/include/
cp build/src/windows/${BUILD_TYPE}/*.lib ../../third-party/build-libs/win/x64/
cp build/src/windows/${BUILD_TYPE}/*.dll ../../third-party/runtime-libs/win/x64/
cd ..
