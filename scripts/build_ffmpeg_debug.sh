#!/bin/sh

ROOT=../external/FFmpeg

pushd $ROOT

./configure --enable-debug=3 --disable-optimizations
make -j8

popd