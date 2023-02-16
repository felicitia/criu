#!/bin/bash

TOOLCHAIN_PATH="/home/yixue/riscv-toolchain/riscv64-unknown-linux-gnu-toolsuite-14.9.0-2022.08.0-x86_64-linux-ubuntu14/bin" 
# TOOLCHAIN_PATH="usr/bin"

TARGET_ARCH="riscv64"
# TARGET_ARCH="aarch64"

CRIU_ROOT_DIR="/home/yixue/criu"


BUILD_ROOT_DIR="/home/yixue/cross-compile-riscv64-artifacts"
# BUILD_ROOT_DIR="/home/yixue/cross-compile-arm64-artifacts"
mkdir -p $BUILD_ROOT_DIR

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