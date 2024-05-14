#!/bin/sh

ROOT=../external/FFmpeg

pushd $ROOT

./configure                                 \
    --cc=clang                              \
    --arch=arm64                            \
    --prefix=/tmp/local/                    \
    --enable-shared                         \
    --enable-debug=3                        \
    --disable-programs                      \
    --disable-optimizations                 \

make -j64
make install

popd