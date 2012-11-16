#!/bin/bash

check_fail () {
    if [[ "$?" -ne "0" ]]
    then
        echo "$1 FAILED"
        exit 1
    fi
}

WDIR=$(pwd)
if [[ ! -e env ]]
then
    mkdir env
fi

# GDSL
wget http://download.gna.org/gdsl/gdsl-1.6.tar.gz
tar xzvf gdsl-1.6.tar.gz
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

# CUnit
wget http://downloads.sourceforge.net/project/cunit/CUnit/2.1-2/CUnit-2.1-2-src.tar.bz2
tar xjvf CUnit-2.1-2-src.tar.bz2
cd CUnit-2.1-2
./configure --prefix=$WDIR/env
check_fail "./configure --prefix=$WDIR/env"
make
check_fail "make"
make install
check_fail "make install"

cd "$WDIR"
rm -r CUnit-2.1-2
rm CUnit-2.1-2-src.tar.bz2

echo
echo "########################################################################"
echo "#"
echo "# The project is now ready to build and run. To do so,"
echo "#"
echo "#  $ make"
echo "#  $ ./smash"
echo "#"
echo "########################################################################"
echo
