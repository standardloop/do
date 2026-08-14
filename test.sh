#!/bin/bash

set -e
REPO_NAME="c-logger"
DYLIB_VERSION=0.0.17
DYLIB_NAME="logger"

otool -L /usr/local/lib/standardloop/libstandardloop-$DYLIB_NAME.dylib | head -n 2 | grep -q "current version $DYLIB_VERSION"
DYLIB_NAME_UPPER_CASE=$(echo "$DYLIB_NAME" | tr '[:lower:]' '[:upper:]')
DYLIB_NAME_UPPER_CASE=${DYLIB_NAME_UPPER_CASE}_H_VERSION
cat /usr/local/include/standardloop/$DYLIB_NAME.h | grep -q "STANDARDLOOP_$DYLIB_NAME_UPPER_CASE \"$DYLIB_VERSION\""
