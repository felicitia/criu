#!/bin/bash

main () {
    # go to the current dir where main.sh is located
    cd "$(dirname "$0")" 

    ./build_protobuf.sh 

    ./build_libnet.sh

    ./build_libnl.sh

    ./build_libcap.sh
}

main