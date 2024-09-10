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

echo "Build type: ${BUILD_TYPE}"
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
patch -p1 < ../../platforms/win/x64/cargs/001.patch
cp include/cargs.h ../../third-party/include/
cmake -G "Visual Studio 17 2022" -DBUILD_SHARED_LIBS=ON -B build
cmake --build build --config ${BUILD_TYPE}
cp build/${BUILD_TYPE}/*.lib ../../third-party/build-libs/win/x64/
cp build/${BUILD_TYPE}/*.dll ../../third-party/runtime-libs/win/x64/
cd ..


#
# build sockpp and copy to external
#

curl -sL https://github.com/fpagliughi/sockpp/archive/${SOCKPP_SHA}.zip -o sockpp.zip
unzip sockpp.zip
cd sockpp-$SOCKPP_SHA
patch -p1 < ../../platforms/win/x64/sockpp/001.patch
cp -r include/sockpp ../../third-party/include/
cmake -G "Visual Studio 17 2022" -B build
cmake --build build --config ${BUILD_TYPE}
cp build/${BUILD_TYPE}/*.lib ../../third-party/build-libs/win/x64/
cp build/${BUILD_TYPE}/*.dll ../../third-party/runtime-libs/win/x64/
cd ..

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