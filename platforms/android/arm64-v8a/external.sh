#!/bin/bash

set -e

source ./platforms/config.sh

echo "Building libraries..."
echo ""

if [[ $(uname) == "Linux" ]]; then
   NUM_PROCS=$(nproc)
elif [[ $(uname) == "Darwin" ]]; then
   NUM_PROCS=$(sysctl -n hw.ncpu)
else
   NUM_PROCS=1
fi

rm -rf external
mkdir external
cd external