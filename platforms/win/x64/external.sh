#!/bin/bash

set -e

if [ -z "${MSYS2_PATH}" ]; then
   MSYS2_PATH="/c/msys64"
fi

echo "MSYS2_PATH: ${MSYS2_PATH}"
echo ""

source ./platforms/config.sh

echo "Building libraries..."
echo "  LIBUSB_SHA: ${LIBUSB_SHA}"
echo "  LIBSERIALPORT_SHA: ${LIBSERIALPORT_SHA}"
echo "  HIDAPI_SHA: ${HIDAPI_SHA}"
echo "  LIBFTDI_SHA: ${LIBFTDI_SHA}"
echo ""

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
sed -i.bak 's/LIBRARY.*libusb-1.0/LIBRARY libusb64-1.0/' libusb/libusb-1.0.def
# remove patch after this is fixed: https://github.com/libusb/libusb/issues/1649#issuecomment-2940138443
cp ../../platforms/win/x64/libusb/libusb_dll.vcxproj msvc
msbuild.exe msvc/libusb_dll.vcxproj \
   -p:TargetName=libusb64-1.0 \
   -p:Platform=x64 \
   -p:Configuration=Release
cp libusb/libusb.h ../../third-party/include/
cp build/v143/x64/Release/libusb_dll/../dll/libusb64-1.0.lib ../../third-party/build-libs/win/x64
cp build/v143/x64/Release/libusb_dll/../dll/libusb64-1.0.dll ../../third-party/runtime-libs/win/x64
cd ..

#
# build libserialport and copy to third-party
#

curl -sL https://github.com/sigrokproject/libserialport/archive/${LIBSERIALPORT_SHA}.tar.gz -o libserialport-${LIBSERIALPORT_SHA}.tar.gz
tar xzf libserialport-${LIBSERIALPORT_SHA}.tar.gz
mv libserialport-${LIBSERIALPORT_SHA} libserialport
cd libserialport
cp libserialport.h ../../third-party/include
msbuild.exe libserialport.sln \
   -p:TargetName=libserialport64 \
   -p:Platform=x64 \
   -p:Configuration=Release
cp x64/Release/libserialport64.lib ../../third-party/build-libs/win/x64
cp x64/Release/libserialport64.dll ../../third-party/runtime-libs/win/x64
cd ..

#
# build hdiapi and copy to third-party
#

curl -sL https://github.com/libusb/hidapi/archive/${HIDAPI_SHA}.tar.gz -o hidapi-${HIDAPI_SHA}.tar.gz
tar xzf hidapi-${HIDAPI_SHA}.tar.gz
mv hidapi-${HIDAPI_SHA} hidapi
cd hidapi
sed -i.bak 's/OUTPUT_NAME "hidapi"/OUTPUT_NAME "hidapi64"/' windows/CMakeLists.txt
cmake \
   -G "Visual Studio 17 2022" \
   -B build
cmake --build build --config ${BUILD_TYPE}
cp -r hidapi ../../third-party/include/
cp build/src/windows/${BUILD_TYPE}/hidapi64.lib ../../third-party/build-libs/win/x64/
cp build/src/windows/${BUILD_TYPE}/hidapi64.dll ../../third-party/runtime-libs/win/x64/
cd ..

#
# build libftdi and copy to third-party
#

curl -sL "http://developer.intra2net.com/git/?p=libftdi;a=snapshot;h=${LIBFTDI_SHA};sf=tgz" -o libftdi-${LIBFTDI_SHA}.tar.gz
tar xzf libftdi-${LIBFTDI_SHA}.tar.gz
mv libftdi-${LIBFTDI_SHA:0:7} libftdi
cd libftdi
sed -i.bak 's/cmake_minimum_required([^)]*)/cmake_minimum_required(VERSION 3.10)/' CMakeLists.txt
sed -i.bak 's/set_target_properties(ftdi1 PROPERTIES VERSION ${VERSION_FIXUP}\.${MINOR_VERSION}\.0 SOVERSION 2)/set_target_properties(ftdi1 PROPERTIES OUTPUT_NAME "ftdi164" VERSION ${VERSION_FIXUP}.${MINOR_VERSION}.0 SOVERSION 2)/' src/CMakeLists.txt
CURRENT_DIR="$(pwd)"
"${MSYS2_PATH}/usr/bin/bash.exe" -l -c "
   cd \"${CURRENT_DIR}\" &&
   cmake \
      -DFTDI_EEPROM=OFF \
      -DEXAMPLES=OFF \
      -DSTATICLIBS=OFF \
      -DLIBUSB_INCLUDE_DIR=../libusb/libusb \
      -DLIBUSB_LIBRARIES=$(pwd)/../libusb/build/v143/x64/Release/libusb_dll/../dll/libusb64-1.0.dll \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -B build &&
   cmake --build build -- -j$(nproc)
"

mkdir -p ../../third-party/include/libftdi1
cp src/ftdi.h ../../third-party/include/libftdi1
cp build/src/libftdi164.dll.a ../../third-party/build-libs/win/x64/libftdi164.lib
cp build/src/libftdi164.dll ../../third-party/runtime-libs/win/x64/
cd ..