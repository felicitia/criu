#!/bin/sh

echo "running script: build_libnl.sh"

. ./config.sh
. ./util.sh

LIBNL_DOWNLOAD_URL="https://www.infradead.org/~tgr/libnl/files/libnl-3.2.25.tar.gz"

# download source code and extract it, including both libnet and libnet-c
download_extract () {
    wget -P $BUILD_ROOT_DIR $LIBNL_DOWNLOAD_URL --quiet

    tarball="$(basename -- $LIBNL_DOWNLOAD_URL)"
    tar -zxf $BUILD_ROOT_DIR/$tarball --directory $BUILD_ROOT_DIR    
}


# build the arm64 version 
build_libnl_arm64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/libnl-3.2.25" 

    mkdir -p arm64_build
    cd arm64_build
    
    CC=aarch64-linux-gnu-gcc \
    CXX=aarch64-linux-gnu-g++ \
    LD=aarch64-linux-gnu-ld \
    AR=aarch64-linux-gnu-ar \
    STRIP=aarch64-linux-gnu-strip \
    ../configure --prefix=$BUILD_ROOT_DIR/arm64_pb_install \
    --enable-static --host=aarch64-unknown-linux-gnu

    make && make install
}

build_libnl_riscv64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/libnl-3.2.25" 

    mkdir -p riscv64_build
    cd riscv64_build
    
    CC=riscv64-unknown-linux-gnu-gcc \
    CXX=riscv64-unknown-linux-gnu-g++ \
    LD=riscv64-unknown-linux-gnu-ld \
    AR=riscv64-unknown-linux-gnu-ar \
    STRIP=riscv64-unknown-linux-gnu-strip \
    ../configure --prefix=$BUILD_ROOT_DIR/riscv64_pb_install \
    --enable-static --host=riscv64-unknown-linux-gnu

    make && make install
}


main () {
    download_extract

    case $TARGET_ARCH in
        "aarch64" | "arm64")
            printf "${BCyan}building libnl for $TARGET_ARCH${Color_Off}\n"
            measure_func_time build_libnl_arm64
            ;;
        
        "riscv64")
            printf "${BCyan}building libnl for $TARGET_ARCH${Color_Off}\n"
            measure_func_time build_libnl_riscv64
            ;;

        *)
            echo "the target architecture $TARGET_ARCH is not supported, exit the program..."
            exit
            ;;
    esac
}

main