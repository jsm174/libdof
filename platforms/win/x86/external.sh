#!/bin/bash

set -e

CARGS_SHA=5949a20a926e902931de4a32adaad9f19c76f251
SOCKPP_SHA=e6c4688a576d95f42dd7628cefe68092f6c5cd0f
LIBSERIALPORT_SHA=fd20b0fc5a34cd7f776e4af6c763f59041de223b

echo "Building libraries..."
echo "  CARGS_SHA: ${CARGS_SHA}"
echo "  SOCKPP_SHA: ${SOCKPP_SHA}"
echo "  LIBSERIALPORT_SHA: ${LIBSERIALPORT_SHA}"
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
cp include/cargs.h ../../third-party/include/
cmake -G "Visual Studio 17 2022" -DBUILD_SHARED_LIBS=ON -A Win32 -B build
cmake --build build --config ${BUILD_TYPE}
cp build/${BUILD_TYPE}/cargs.lib ../../third-party/build-libs/win/x86/
cp build/${BUILD_TYPE}/cargs.dll ../../third-party/runtime-libs/win/x86/
cd ..


#
# build sockpp and copy to external
#

curl -sL https://github.com/fpagliughi/sockpp/archive/${SOCKPP_SHA}.zip -o sockpp.zip
unzip sockpp.zip
cd sockpp-$SOCKPP_SHA
cp -r include/sockpp ../../third-party/include/
cmake -G "Visual Studio 17 2022" -A Win32 -B build
cmake --build build --config ${BUILD_TYPE}
cp build/${BUILD_TYPE}/sockpp.lib ../../third-party/build-libs/win/x86/
cp build/${BUILD_TYPE}/sockpp.dll ../../third-party/runtime-libs/win/x86/
cd ..

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
