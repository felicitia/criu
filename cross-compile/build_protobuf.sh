#!/bin/sh

PROTOBUF_DOWNLOAD_URL="https://github.com/protocolbuffers/protobuf/releases/download/v3.5.1/protobuf-all-3.5.1.tar.gz"

PROTOBUF_BUILD_ROOT_DIR="/home/yixue/cross-compile-arm64-artifacts/protobuf"
mkdir -p $PROTOBUF_BUILD_ROOT_DIR


# build a native (x86_64) version of the protobuf libraries and compiler (protoc)
build_protobuf_x86_64 () {
    # go to the folder where the extracted files are
    cd "$PROTOBUF_BUILD_ROOT_DIR/protobuf-3.5.1" 

    mkdir -p x86_64_build
    cd x86_64_build

    ../configure --prefix=$PROTOBUF_BUILD_ROOT_DIR/x86_64_pb_install
    
    make install -j16
}

# build the arm64 version of the protobuf libraries
build_protobuf_arm64 () {
    # go to the folder where the extracted files are
    cd "$PROTOBUF_BUILD_ROOT_DIR/protobuf-3.5.1" 

    mkdir -p arm64_build
    cd arm64_build
    
    CC=aarch64-linux-gnu-gcc \
    CXX=aarch64-linux-gnu-g++ \
    ../configure --host=aarch64-linux \
    --prefix=$PROTOBUF_BUILD_ROOT_DIR/arm64_pb_install \
    --with-protoc=$PROTOBUF_BUILD_ROOT_DIR/x86_64_pb_install/bin/protoc
    
    make install -j16
}

measure_func_time () {
    start=$(date +%s.%N)
    ($1) 
    duration=$(echo "$(date +%s.%N) - $start" | bc)
    execution_time=`printf "%.2f seconds" $duration`
    echo "$1 Execution Time: $execution_time"
}

# download source code and extract it
wget -P $PROTOBUF_BUILD_ROOT_DIR $PROTOBUF_DOWNLOAD_URL --quiet
tarball="$(basename -- $PROTOBUF_DOWNLOAD_URL)"
tar -zxf $PROTOBUF_BUILD_ROOT_DIR/$tarball --directory $PROTOBUF_BUILD_ROOT_DIR

measure_func_time build_protobuf_x86_64

measure_func_time build_protobuf_arm64


