#!/bin/bash

set -e

LIBSERIALPORT_SHA=21b3dfe5f68c205be4086469335fd2fc2ce11ed2
HIDAPI_SHA=d0732cda906ad07b7e1ef93f1919035643620435

if [ -z "${BUILD_TYPE}" ]; then
   BUILD_TYPE="Release"
fi

echo "Build type: ${BUILD_TYPE}"
echo ""