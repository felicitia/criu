#!/bin/bash

printf "${BCyan}running script: build_compel_test.sh${Color_Off}\n"

. ./config.sh

cd $CRIU_ROOT_DIR

export PATH=$BUILD_ROOT_DIR/x86_64_pb_install/bin:$PATH
# export PKG_CONFIG_PATH=$BUILD_ROOT_DIR/arm64_pb_install/lib/pkgconfig:$PKG_CONFIG_PATH
export PKG_CONFIG_PATH=$BUILD_ROOT_DIR/riscv64_pb_install/lib/pkgconfig:$PKG_CONFIG_PATH

CFLAGS=$(pkg-config --cflags libprotobuf-c)
CFLAGS+=" -I$INCLUDE_DIR_CC -L$LIB_DIR_CC"


LDFLAGS=$(pkg-config --libs libprotobuf-c)
# LDFLAGS+=" -rpath /usr/aarch64-linux-gnu/lib"
LDFLAGS+=" -rpath $TOOLCHAIN_LIB_DIR"

# V=1 \
# ARCH=aarch64 CROSS_COMPILE=aarch64-linux-gnu- \
# CFLAGS=$(pkg-config --cflags libprotobuf-c) \
# LDFLAGS=$LDFLAGS \
# make -f Makefile-CC


cd $CRIU_ROOT_DIR/compel/test/infect
# V=1 \
ARCH=riscv64 \
CROSS_COMPILE=riscv64-unknown-linux-gnu- \
CFLAGS=$CFLAGS \
LDFLAGS=$LDFLAGS \
make -f Makefile-CC

cd $CRIU_ROOT_DIR/compel/test/rsys
# V=1 \
ARCH=riscv64 \
CROSS_COMPILE=riscv64-unknown-linux-gnu- \
CFLAGS=$CFLAGS \
LDFLAGS=$LDFLAGS \
make -f Makefile-CC

cd $CRIU_ROOT_DIR/compel/test/fdspy
# V=1 \
ARCH=riscv64 \
CROSS_COMPILE=riscv64-unknown-linux-gnu- \
CFLAGS=$CFLAGS \
LDFLAGS=$LDFLAGS \
make -f Makefile-CC

cd $CRIU_ROOT_DIR/compel/test/stack
# V=1 \
ARCH=riscv64 \
CROSS_COMPILE=riscv64-unknown-linux-gnu- \
CFLAGS=$CFLAGS \
LDFLAGS=$LDFLAGS \
make -f Makefile-CC