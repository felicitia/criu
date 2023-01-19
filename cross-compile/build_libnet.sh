#!/bin/sh

echo "running script: build_libnet.sh"

. ./config.sh
. ./util.sh

LIBNET_DOWNLOAD_URL="https://github.com/libnet/libnet/releases/download/v1.2/libnet-1.2.tar.gz"

LIBNET_BUILD_ROOT_DIR="/home/yixue/cross-compile-arm64-artifacts/libnet"
mkdir -p $LIBNET_BUILD_ROOT_DIR

# download source code and extract it, including both libnet and libnet-c
download_extract () {
    wget -P $LIBNET_BUILD_ROOT_DIR $LIBNET_DOWNLOAD_URL --quiet

    tarball="$(basename -- $LIBNET_DOWNLOAD_URL)"
    tar -zxf $LIBNET_BUILD_ROOT_DIR/$tarball --directory $LIBNET_BUILD_ROOT_DIR    
}


# build the arm64 version of the protobuf libraries
build_libnet_arm64 () {
    # go to the folder where the extracted files are
    cd "$LIBNET_BUILD_ROOT_DIR/libnet-1.2" 

    mkdir -p arm64_build
    cd arm64_build
    
    CC=aarch64-linux-gnu-gcc \
    CXX=aarch64-linux-gnu-g++ \
    ../configure --prefix=$LIBNET_BUILD_ROOT_DIR/arm64_pb_install \
    --disable-shared --enable-static --host=aarch64-linux

    make && make install
}

# build the arm64 version of the protobuf libraries
build_libnet_riscv64 () {
    # go to the folder where the extracted files are
    cd "$LIBNET_BUILD_ROOT_DIR/libnet-1.2" 

    mkdir -p riscv64_build
    cd riscv64_build
    
    CC=riscv64-unknown-linux-gnu-gcc \
    CXX=riscv64-unknown-linux-gnu-g++ \
    ../configure --prefix=$LIBNET_BUILD_ROOT_DIR/riscv64_pb_install \
    --disable-shared --enable-static --host=riscv64-unknown-linux

    make && make install
}

main () {
    download_extract

    case $TARGET_ARCH in
        "aarch64" | "arm64")
            echo "building libnet for $TARGET_ARCH"
            measure_func_time build_libnet_arm64
            ;;
        
        "riscv64")
            echo "building libnet for $TARGET_ARCH"
            measure_func_time build_libnet_riscv64
            ;;

        *)
            echo "the target architecture $TARGET_ARCH is not supported, exit the program..."
            exit
            ;;
    esac
}

main