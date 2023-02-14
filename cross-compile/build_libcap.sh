#!/bin/sh

echo "running script: build_libcap.sh"

. ./config.sh
. ./util.sh

LIBCAP_DOWNLOAD_URL="https://www.tcpdump.org/release/libpcap-1.10.3.tar.gz"

# download source code and extract it, including both libnet and libnet-c
download_extract () {
    wget -P $BUILD_ROOT_DIR $LIBCAP_DOWNLOAD_URL --quiet

    tarball="$(basename -- $LIBCAP_DOWNLOAD_URL)"
    tar -zxf $BUILD_ROOT_DIR/$tarball --directory $BUILD_ROOT_DIR    
}


# build the arm64 version 
build_libcap_arm64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/libpcap-1.10.3" 

    mkdir -p arm64_build
    cd arm64_build
    
    CC=aarch64-linux-gnu-gcc \
    CXX=aarch64-linux-gnu-g++ \
    CFLAGS="-Os" \
    ../configure --prefix=$BUILD_ROOT_DIR/arm64_pb_install \
    --enable-static --host=aarch64-unknown-linux-gnu --with-pcap=linux

    make && make install
}

# build the riscv64 version 
build_libcap_riscv64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/libpcap-1.10.3" 

    mkdir -p riscv64_build
    cd riscv64_build
    
    CC=riscv64-unknown-linux-gnu-gcc \
    CXX=riscv64-unknown-linux-gnu-g++ \
    CFLAGS="-Os" \
    ../configure --prefix=$BUILD_ROOT_DIR/riscv64_pb_install \
    --enable-static --host=riscv64-unknown-linux-gnu --with-pcap=linux

    make && make install
}

main () {
    download_extract

    case $TARGET_ARCH in
        "aarch64" | "arm64")
            printf "${BCyan}building libcap for $TARGET_ARCH${Color_Off}\n"
            measure_func_time build_libcap_arm64
            ;;
        
        "riscv64")
            printf "${BCyan}building libcap for $TARGET_ARCH${Color_Off}\n"
            measure_func_time build_libcap_riscv64
            ;;

        *)
            echo "the target architecture $TARGET_ARCH is not supported, exit the program..."
            exit
            ;;
    esac
}

main