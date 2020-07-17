#!/bin/bash

make clean

rm -f kylin-scanner.pro.user \
    .qmake.stash \
    Makefile \
    src/Makefile \
    src/kylin-scanner \
    debian/debhelper-build-stamp \
    debian/files \
    debian/kylin-scanner.substvars

rm -rf debian/.debhelper/ \
    debian/kylin-scanner

