#!/bin/sh

INSTDIR=tmp/gaia2

rm -fr $INSTDIR

# copy python bindings
mkdir -p $INSTDIR/python

cp -r ../../src/bindings/pygaia/ $INSTDIR/python/gaia2
cp Gaia2Python/libGaia2Python.1.0.0.dylib $INSTDIR/python/gaia2/_gaia2.so
cp ../../src/bindings/gaia2.py $INSTDIR/python/gaia2/__init__.py

# copy gaia tools
mkdir -p $INSTDIR/tools

cp gaiainfo/gaiainfo $INSTDIR/tools
cp gaiamerge/gaiamerge $INSTDIR/tools
cp ../../src/tools/gaiafusion.py $INSTDIR/tools/gaiafusion
