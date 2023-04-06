## Instructions for Cross Compiling

1. modify the paths in `cross-compile/config.sh`

2. run `cross-compile/build_required_deps.sh` to cross compile CRIU's required dependencies (e.g., protobuf, libcap). Alternatively, you can run each build script separately (e.g., run `build_protobuf.sh` to cross compile protobuf and protobuf-C). You can also modify the version of CRIU's dependencies in each build script.

Notes: These required packages are ported to RISC-V already. Contact Yixue Zhao for the artifacts if you don't want to build on your own.

3. run `cross-compile/build_criu.sh` to cross compile CRIU
