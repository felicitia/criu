# Cross Compiling CRIU

## Toolchain install

`sudo apt install gcc make gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu g++-aarch64-linux-gnu`


## Cross compile protobuf

Script to automate the process: https://github.com/felicitia/criu/blob/criu-dev/cross-compile/build_protobuf.sh

Reference: https://review.mlplatform.org/plugins/gitiles/ml/armnn/+/refs/heads/branches/armnn_19_05/BuildGuideCrossCompilation.md

1.	download source code

`wget https://github.com/protocolbuffers/protobuf/releases/download/v3.5.1/protobuf-all-3.5.1.tar.gz` 

2.	extract

`tar -zxvf protobuf-all-3.5.1.tar.gz`

3. build a native (x86_64) version of the protobuf libraries and compiler (protoc) -- Execution Time: 118s

```shell
cd protobuf-all-3.5.1

mkdir x86_64_build

cd x86_64_build

../configure --prefix=$HOME/armnn-devenv/google/x86_64_pb_install

make install -j16
```

4. build the aarch64 version of the protobuf libraries -- Execution Time: 123s

```shell
cd protobuf-all-3.5.1

mkdir aarch64_build

cd aarch64_build

CC=aarch64-linux-gnu-gcc \
CXX=aarch64-linux-gnu-g++ \
../configure --host=aarch64-linux \
--prefix=$HOME/armnn-devenv/google/arm64_pb_install \
--with-protoc=$HOME/armnn-devenv/google/x86_64_pb_install/bin/protoc

make install -j16
```

The aarch64 version will be installed at `/home/yixue/armnn-devenv/google/arm64_pb_install`

5. to test it on aarch64 machine, run the following

`LD_LIBRARY_PATH=/home/yixue/Documents/armnn-devenv/google/arm64_pb_install/lib bin/protoc
`

## Cross Compile protobuf-c

Script to automate the process: https://github.com/felicitia/criu/blob/criu-dev/cross-compile/build_protobuf.sh

Reference: https://github.com/checkpoint-restore/criu/blob/criu-dev/Documentation/HOWTO.cross-compile, https://gist.github.com/RamseyK/a7a534ffbda34fff08fb0544dde3baa7


1. download source code

`wget https://github.com/protobuf-c/protobuf-c/releases/download/v1.4.1/protobuf-c-1.4.1.tar.gz`

2.	extract

`tar -zxvf protobuf-c-1.4.1.tar.gz`

3. Configure protobuf-c for the architecture x86, and then compile and install it -- Execution Time: 70s

```shell
cd protobuf-c-1.4.1
mkdir x86_64_build
cd x86_64_build

export PATH=$PATH:$HOME/armnn-devenv/google/x86_64_pb_install/bin
export PKG_CONFIG_PATH=$HOME/armnn-devenv/google/x86_64_pb_install/lib/pkgconfig

# configure protobuf-c to install at the same location as protobuf
CPPFLAGS=`pkg-config --cflags protobuf` LDFLAGS=`pkg-config --libs protobuf` ../configure --prefix=$HOME/cross-compile-arm64-artifacts/protobuf/x86_64_pb_install --disable-shared --enable-static

make && make install

```

4. build the aarch64 version of the protobuf-c libraries, and then compile and install it -- Execution Time: 14s

```shell
cd protobuf-c-1.4.1
mkdir arm64_build
cd arm64_build

# change pkg config path
export PKG_CONFIG_PATH=$HOME/cross-compile-arm64-artifacts/protobuf/arm64_pb_install/lib/pkgconfig

CC=aarch64-linux-gnu-gcc \
CXX=aarch64-linux-gnu-g++ \
CPPFLAGS=`pkg-config --cflags protobuf` \
LDFLAGS=`pkg-config --libs protobuf` \
../configure --prefix=$HOME/cross-compile-arm64-artifacts/protobuf/arm64_pb_install \
--disable-shared --enable-static --disable-protoc --host=aarch64-linux 

make && make install

```

## Cross Compile CRIU

```shell
cd criu

V=1 ARCH=aarch64 CROSS_COMPILE=aarch64-linux-gnu \
CFLAGS=`pkg-config --cflags libprotobuf-c` \
LDFLAGS="`pkg-config --libs libprotobuf-c`" make

```