#!/bin/bash

make clean

rm -f kylin-scanner.pro.user \
    .qmake.stash \
    Makefile \
    src/Makefile \
    src/kylin-scanner

rm -rf debian/.debhelper/ \
    debian/kylin-scanner

