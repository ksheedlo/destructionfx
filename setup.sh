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

# Check, a C unit testing framework
wget http://downloads.sourceforge.net/project/check/check/0.9.8/check-0.9.8.tar.gz
tar xzvf check-0.9.8.tar.gz
cd check-0.9.8
./configure --prefix=$WDIR/env
check_fail "./configure --prefix=$WDIR/env"
make 
check_fail "make"
make install 
check_fail "make install"

cd "$WDIR"
rm check-0.9.8.tar.gz
rm -r check-0.9.8

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
