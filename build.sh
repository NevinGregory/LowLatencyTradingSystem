#! /bin/bash

CMAKE="$(command -v cmake || echo /usr/bin/cmake)"
NINJA="$(command -v ninja || echo /usr/bin/ninja)"

mkdir -p cmake-build-release
$CMAKE -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=$NINJA -G Ninja -S . -B cmake-build-release

$CMAKE --build cmake-build-release --target clean -j 4
$CMAKE --build cmake-build-release --target all -j 4
