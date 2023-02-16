#!/bin/bash

printf "${BCyan}running script: build_criu.sh${Color_Off}\n"

. ./config.sh

cd $CRIU_ROOT_DIR

export PATH=$BUILD_ROOT_DIR/x86_64_pb_install/bin:$PATH
# export PKG_CONFIG_PATH=$BUILD_ROOT_DIR/arm64_pb_install/lib/pkgconfig:$PKG_CONFIG_PATH
export PKG_CONFIG_PATH=$BUILD_ROOT_DIR/riscv64_pb_install/lib/pkgconfig:$PKG_CONFIG_PATH

make mrproper

file="/home/yixue/criu/compel/plugins/include/uapi/std/syscall-aux.S"
if [ -f "$file" ] ; then
    rm "$file"
fi

file="/home/yixue/criu/compel/plugins/include/uapi/std/syscall-aux.h"
if [ -f "$file" ] ; then
    rm "$file"
fi

LDFLAGS=$(pkg-config --libs libprotobuf-c)
# LDFLAGS+=" -rpath /usr/aarch64-linux-gnu/lib"
LDFLAGS+=" -rpath /home/yixue/riscv-toolchain/riscv64-unknown-linux-gnu-toolsuite-14.9.0-2022.08.0-x86_64-linux-ubuntu14/sysroot/lib"

# V=1 \
# ARCH=aarch64 CROSS_COMPILE=aarch64-linux-gnu- \
# CFLAGS=$(pkg-config --cflags libprotobuf-c) \
# LDFLAGS=$LDFLAGS \
# make -f Makefile-CC

V=1 \
ARCH=riscv64 CROSS_COMPILE=riscv64-unknown-linux-gnu- \
CFLAGS=$(pkg-config --cflags libprotobuf-c) \
LDFLAGS=$LDFLAGS \
make -f Makefile-CC

