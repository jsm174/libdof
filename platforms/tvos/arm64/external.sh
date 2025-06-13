#!/bin/bash

set -e

source ./platforms/config.sh

echo "Building libraries..."
echo ""

NUM_PROCS=$(sysctl -n hw.ncpu)

rm -rf external
mkdir external
cd external