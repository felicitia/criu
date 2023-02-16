# Cross Compiling CRIU


Note that the scripts are more up to date than the notes.

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


## Cross Compile libnet

Reference: https://github.com/libnet/libnet

Script: https://github.com/felicitia/criu/blob/criu-dev/cross-compile/build_libnet.sh

1. download  & extract like previoius steps

2. build aarch64 version of libnet, compile and install -- Execution Time: 25s

```shell
cd libnet-1.2
mkdir arm64_build
cd arm64_build

CC=aarch64-linux-gnu-gcc \
CXX=aarch64-linux-gnu-g++ \
../configure --prefix=$HOME/cross-compile-arm64-artifacts/libnet/arm64_pb_install \
--disable-shared --enable-static --host=aarch64-linux

make && make install

```

## Cross Compile libcap

Ref: https://stackoverflow.com/questions/59507388/cross-compile-libpcap-dev-aarch64, https://emreboy.wordpress.com/2013/03/02/cross-compile-libpcap-source-code/

Script: https://github.com/felicitia/criu/blob/criu-dev/cross-compile/build_libcap.sh


## Cross Compile libnl-3

Ref: https://medium.com/@vikasv210/libnl-cross-compilation-b0df1d52fd12, https://stackoverflow.com/questions/52200502/error-while-trying-to-cross-compile-libnl-for-android-arm64

Script: https://github.com/felicitia/criu/blob/criu-dev/cross-compile/build_libnl.sh

Execution Time: 112s

## Cross Compile CRIU

Script: https://github.com/felicitia/criu/blob/criu-dev/cross-compile/build_criu.sh

## Notes on the issues during cross compiling (X86 -> aarch64):

1. need to delete `/criu/compel/plugins/include/uapi/std/syscall-aux.h` and `syscall-aux.S` in the same folder if existed before build 

Solution: delete those files. this step is done in `build_criu.sh`

2. redefinition of struct tcp_repair_window. It's been reported that both tcp_repair_opt and TCP_ enum already shipped in netinet/tcp.h system header by some distros.

Solution: check `soccr/soccr.h` and add the following.

```c
#define CONFIG_HAS_TCP_REPAIR_WINDOW
#define CONFIG_HAS_TCP_REPAIR
```


3. Similar issue to 2. Add the following definitions.

```c
/* in criu/include/memfd.h */
#define CONFIG_HAS_MEMFD_CREATE
```

```c
/* in criu/include/linux/rseq.h */
#ifndef CONFIG_HAS_NO_LIBC_RSEQ_DEFS
#define CONFIG_HAS_NO_LIBC_RSEQ_DEFS
#endif	
```


4. disabled SELinux in `Makefile.config` by commenting out the following. 

```Makefile
#        LIBS_FEATURES	+= -lselinux
#      FEATURE_DEFINES	+= -DCONFIG_HAS_SELINUX
```

5. undefined reference to `pthread_rwlock_wrlock` 

Solution: dynamically build libnl without `--disable-shared` flag (to generated .so)

6. `aarch64-linux-gnu-ld` needs to add `-rpath` 

Solution: added `-rpath` in script `build_criu.sh` but the LDFLAGS are passed to gcc as well (see `criu/Makefile` Line 88, and `lib/Makefile` Line 22). Changed to the following.

```Makefile
	/* in criu/Makefile */
	# $(Q) $(CC) $(CFLAGS) $^ $(LIBS) $(WRAPFLAGS) $(LDFLAGS) $(GMONLDOPT) -rdynamic -o $@ 
	$(Q) $(CC) $(CFLAGS) $^ $(LIBS) $(WRAPFLAGS) $(GMONLDOPT) -rdynamic -o $@ #yixue change


	/* in lib/Makefile */
	# $(Q) $(CC) -shared $(cflags-so) -o $@ $^ $(ldflags-so) $(LDFLAGS)
	$(Q) $(CC) -shared $(cflags-so) -o $@ $^ $(ldflags-so) # yixue change
```

7. `/home/yixue/cross-compile-arm64-artifacts/arm64_pb_install/bin/protoc: 1: ELF��@U@8: not found
/home/yixue/cross-compile-arm64-artifacts/arm64_pb_install/bin/protoc: 1: Syntax error: Unterminated quoted string -- need to use X86 `protoc` command.` 

Solution: The makefiles normally invoke the protoc executable that they just built in order to build tests. When cross-compiling, the protoc executable may not be executable on the host machine. In this case, you must build a copy of protoc for the host machine first and use that executable (in our case, we need to use X86 version of `protoc` executable).


8. `fatal error: sys/capability.h: No such file or directory`

Solution: `sudo apt-get install libcap-dev` (although libcap is already cross compiled, but the header files can be found by this solution)

9. compiling didn't pass the check on the required packages and abort.

Solution: criu checks the required packages on the host machine, but is not aware of cross-compiling environment. Quick fix is to disable the check as the following.

```Makefile
/* in criu/Makefile.packages, comment out the abort */
#	$(error Compilation aborted)
```

10. When headers are not found, need to tell the compiler to include the header files by the following.

```Makefile
/* in top-level Makefile-CC, update CFLAGS */
CFLAGS		+= -O2 -g -I/home/yixue/cross-compile-arm64-artifacts/arm64_pb_install/include -I/usr/aarch64-linux-gnu/include -L/home/yixue/cross-compile-arm64-artifacts/arm64_pb_install/lib
```


11. When running on ARM machine, `./criu: error while loading shared libraries: libprotobuf-c.so.1: cannot open shared object file: No such file or directory`

Solution: export LD_LIBRARY_PATH （although only doing that didn't work for me), but `ldconfig` worked (do `sudo ldconfig /home/yixue/Documents/cross-compile-arm64-artifacts/arm64_pb_install/lib/`).

## Notes on the issues during cross compiling (X86 -> RISC-V):

1. ` undefined reference to `google::protobuf::internal::Release_CompareAndSwap(long volatile*, long, long)'`

Solution: https://github.com/protocolbuffers/protobuf/issues/3937

2. `configure: error: in /home/yixue/cross-compile-riscv64-artifacts/protobuf-c-1.4.1/riscv64_build':
configure: error: C compiler cannot create executables`

Solution: commented out `pkg-config` as shown below. The reason is that `pkd-config` is not found in the PATH environment variable. When crossing compiling to aarch64, it can be found in `urs/bin` (same path as TOOLCHAIN_PATH) so it's already included. If needed, we'll add back the `pkg-config` and update the PATH environment variable to find it for RISC-V cross compiling environment.

```shell
	# in build_protobuf.sh
	CC=riscv64-unknown-linux-gnu-gcc \
    CXX=riscv64-unknown-linux-gnu-g++ \
    # CPPFLAGS="pkg-config --cflags protobuf" \
    # LDFLAGS="pkg-config --libs protobuf" \
    ../configure --prefix=$BUILD_ROOT_DIR/riscv64_pb_install \
    --enable-static --disable-protoc --host=riscv64-unknown-linux-gnu
```