#!/bin/bash

set -e

LIBSERIALPORT_SHA=21b3dfe5f68c205be4086469335fd2fc2ce11ed2
HIDAPI_SHA=d6b2a974608dec3b76fb1e36c189f22b9cf3650c

if [ -z "${BUILD_TYPE}" ]; then
   BUILD_TYPE="Release"
fi

echo "Build type: ${BUILD_TYPE}"
echo ""
