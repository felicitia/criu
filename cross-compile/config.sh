#!/bin/bash

TOOLCHAIN_PATH="/usr/bin" 

CRIU_ROOT_DIR="/home/yixue/criu"

TARGET_ARCH="aarch64"

BUILD_ROOT_DIR="/home/yixue/cross-compile-arm64-artifacts"
mkdir -p $BUILD_ROOT_DIR

export PATH=$PATH:$TOOLCHAIN_PATH
