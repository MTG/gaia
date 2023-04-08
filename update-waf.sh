#!/bin/bash

VERSION=2.0.24
TMP_DIR=./waf-repo

git clone --depth=1 --branch=waf-${VERSION} -c advice.detachedHead=false https://gitlab.com/ita1024/waf.git ${TMP_DIR}
${TMP_DIR}/waf-light --tools=swig,c_emscripten,xcode6 --make-waf -t ${TMP_DIR}
cp ${TMP_DIR}/waf .
rm -rf ${TMP_DIR}
