#!/bin/bash

check_fail () {
    if [[ "$?" -ne "0" ]]
    then
        echo "$1 FAILED"
        exit 1
    fi
}

wget http://download.gna.org/gdsl/gdsl-1.6.tar.gz
tar xzvf gdsl-1.6.tar.gz

WDIR=$(pwd)
if [[ ! -e env ]]
then
    mkdir env
fi

cd gdsl-1.6
./configure --prefix=$WDIR/env
check_fail "./configure --prefix=$WDIR/env"
make
check_fail "make"
make install
check_fail "make install"

cd "$WDIR"
rm gdsl-1.6.tar.gz
rm -r gdsl-1.6
