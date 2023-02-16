#!/bin/bash

main () {
    # go to the current dir where main.sh is located
    cd "$(dirname "$0")" 

    ./build_protobuf.sh 

    ./build_libnet.sh

    ./build_nl.sh

    ./build_libcap.sh

    ./build_criu.sh
}

main