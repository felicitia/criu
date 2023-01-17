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

