#!/bin/sh

ROOT=../external/FFmpeg

pushd $ROOT

./configure                                 \
    --cc=clang                              \
    --prefix=local                          \
    --disable-programs --enable-shared      \
    --enable-debug=3 --disable-optimizations

make -j64
make install

popd