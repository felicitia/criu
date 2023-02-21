#!/bin/sh

echo "running script: build_protobuf.sh"

. ./config.sh
. ./util.sh

PROTOBUF_DOWNLOAD_URL="https://github.com/protocolbuffers/protobuf/releases/download/v3.5.1/protobuf-all-3.5.1.tar.gz"
PROTOBUF_C_DOWNLOAD_URL="https://github.com/protobuf-c/protobuf-c/releases/download/v1.4.1/protobuf-c-1.4.1.tar.gz"


# build a native (x86_64) version of the protobuf libraries and compiler (protoc)
build_protobuf_x86_64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/protobuf-3.5.1" 

    mkdir -p x86_64_build
    cd x86_64_build

    ../configure --prefix=$BUILD_ROOT_DIR/x86_64_pb_install
    
    make install -j16
}

# build a native (x86_64) version of the protobuf-c
build_protobufC_x86_64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/protobuf-c-1.4.1"
    mkdir -p x86_64_build
    cd x86_64_build

    export PATH=$PATH:$BUILD_ROOT_DIR/x86_64_pb_install/bin
    export PKG_CONFIG_PATH=$BUILD_ROOT_DIR/x86_64_pb_install/lib/pkgconfig:$PKG_CONFIG_PATH

    # configure protobuf-c to install at the same location as protobuf
    CPPFLAGS=$(pkg-config --cflags protobuf) \
    LDFLAGS=$(pkg-config --libs protobuf) \
    ../configure --prefix=$BUILD_ROOT_DIR/x86_64_pb_install \
    --enable-static

    make && make install
}


# build the arm64 version of the protobuf libraries
build_protobuf_arm64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/protobuf-3.5.1" 

    mkdir -p arm64_build
    cd arm64_build
    
    CC=aarch64-linux-gnu-gcc \
    CXX=aarch64-linux-gnu-g++ \
    ../configure --host=aarch64-linux \
    --prefix=$BUILD_ROOT_DIR/arm64_pb_install \
    --with-protoc=$BUILD_ROOT_DIR/x86_64_pb_install/bin/protoc
    
    make install -j16
}


# build the riscv64 version of the protobuf libraries
build_protobuf_riscv64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/protobuf-3.5.1" 

    mkdir -p riscv64_build
    cd riscv64_build
    
    CC=riscv64-unknown-linux-gnu-gcc \
    CXX=riscv64-unknown-linux-gnu-g++ \
    ../configure --host=riscv64-unknown-linux \
    --prefix=$BUILD_ROOT_DIR/riscv64_pb_install \
    --with-protoc=$BUILD_ROOT_DIR/x86_64_pb_install/bin/protoc
    
    make install -j16
}

# build the arm64 version of the protobuf-c
build_protobufC_arm64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/protobuf-c-1.4.1"
    mkdir -p arm64_build
    cd arm64_build

    # change pkg config path
    export PKG_CONFIG_PATH=$BUILD_ROOT_DIR/arm64_pb_install/lib/pkgconfig:$PKG_CONFIG_PATH

    CC=aarch64-linux-gnu-gcc \
    CXX=aarch64-linux-gnu-g++ \
    CPPFLAGS=$(pkg-config --cflags protobuf) \
    LDFLAGS=$(pkg-config --libs protobuf) \
    ../configure --prefix=$BUILD_ROOT_DIR/arm64_pb_install \
    --enable-static --disable-protoc --host=aarch64-unknown-linux-gnu

    make && make install
}

# build the riscv64 version of the protobuf-c
build_protobufC_riscv64 () {
    # go to the folder where the extracted files are
    cd "$BUILD_ROOT_DIR/protobuf-c-1.4.1"
    mkdir -p riscv64_build
    cd riscv64_build

    # change pkg config path
    export PKG_CONFIG_PATH=$BUILD_ROOT_DIR/riscv64_pb_install/lib/pkgconfig:$PKG_CONFIG_PATH

    CC=riscv64-unknown-linux-gnu-gcc \
    CXX=riscv64-unknown-linux-gnu-g++ \
    # CPPFLAGS="pkg-config --cflags protobuf" \
    # LDFLAGS="pkg-config --libs protobuf" \
    ../configure --prefix=$BUILD_ROOT_DIR/riscv64_pb_install \
    --enable-static --disable-protoc --host=riscv64-unknown-linux-gnu

    make && make install
}


# download source code and extract it, including both protobuf and protobuf-c
download_extract () {
    wget -P $BUILD_ROOT_DIR $PROTOBUF_DOWNLOAD_URL --quiet
    wget -P $BUILD_ROOT_DIR $PROTOBUF_C_DOWNLOAD_URL --quiet

    tarball="$(basename -- $PROTOBUF_DOWNLOAD_URL)"
    tar -zxf $BUILD_ROOT_DIR/$tarball --directory $BUILD_ROOT_DIR
    
    tarball="$(basename -- $PROTOBUF_C_DOWNLOAD_URL)"
    tar -zxf $BUILD_ROOT_DIR/$tarball --directory $BUILD_ROOT_DIR
}


main () {
    
    download_extract
    
    measure_func_time build_protobuf_x86_64

    measure_func_time build_protobufC_x86_64 

    case $TARGET_ARCH in
        "aarch64" | "arm64")
            printf "${BCyan}building protobuf for $TARGET_ARCH${Color_Off}\n"
            measure_func_time build_protobuf_arm64
            printf "${BCyan}building protobuf-C for $TARGET_ARCH${Color_Off}\n"
            measure_func_time build_protobufC_arm64
            ;;
        
        "riscv64")
            printf "${BCyan}building protobuf for $TARGET_ARCH${Color_Off}\n"
            measure_func_time build_protobuf_riscv64
            printf "${BCyan}building protobuf-C for $TARGET_ARCH${Color_Off}\n"
            measure_func_time build_protobufC_riscv64
            ;;

        *)
            echo "the target architecture $TARGET_ARCH is not supported, exit the program..."
            exit
            ;;
    esac

}

main