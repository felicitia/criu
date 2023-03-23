#!/bin/bash

TOOLCHAIN_ROOT="/home/yixue/riscv-toolchain/riscv64-unknown-linux-gnu-toolsuite-14.9.0-2022.12.0"
# the path that contains cross compiling toolchain binaries, e.g., riscv64-unknown-linux-gnu-gcc, riscv64-unknown-linux-gnu-ld
TOOLCHAIN_PATH="$TOOLCHAIN_ROOT/bin" 
# TOOLCHAIN_PATH="usr/bin"

TARGET_ARCH="riscv64"
# TARGET_ARCH="aarch64"

# the root directory that contains CRIU's source code
CRIU_ROOT_DIR="/home/yixue/criu-riscv64-CC"

# the root directory that will contain the cross compiled artifacts (initially empty)
# e.g., the RISC-V binaries of protobuf (CRIU's required package)
BUILD_ROOT_DIR="/home/yixue/cross-compile-riscv64-artifacts"
# BUILD_ROOT_DIR="/home/yixue/cross-compile-arm64-artifacts"
mkdir -p $BUILD_ROOT_DIR

# no need to change it, unless you changed the build scripts for CRIU's dependencies (e.g., build_protobuf.sh)
# the path should be consistent with the prefix specified in the build scripts (e.g., build_protobuf.sh)
INCLUDE_DIR_CC="$BUILD_ROOT_DIR/riscv64_pb_install/include"
LIB_DIR_CC="$BUILD_ROOT_DIR/riscv64_pb_install/lib"
TOOLCHAIN_INCLUDE_DIR="$TOOLCHAIN_ROOT/sysroot/usr/include"

# the directory that contains the toolchain libraries, e.g., libpthread.so.0
TOOLCHAIN_LIB_DIR="$TOOLCHAIN_ROOT/sysroot/lib"

export PATH=$TOOLCHAIN_PATH:$PATH

# Reset
Color_Off='\033[0m'       # Text Reset

# Bold
BBlack='\033[1;30m'       # Black
BRed='\033[1;31m'         # Red
BGreen='\033[1;32m'       # Green
BYellow='\033[1;33m'      # Yellow
BBlue='\033[1;34m'        # Blue
BPurple='\033[1;35m'      # Purple
BCyan='\033[1;36m'        # Cyan
BWhite='\033[1;37m'       # White