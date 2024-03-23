#!/bin/bash

cflags="-Wall -O3 -g -std=gnu++11 -fno-strict-aliasing -Isrc"
lflags="-lSDL2 -lm"


platform="unix"
out="Elementxt"
compiler="g++"
mcompiler="moonc"
cflags="$cflags -DLUA_USE_POSIX"
lflags="$lflags -o $out"

if command -v ccache >/dev/null; then
  compiler="ccache $compiler"
fi

echo "compiling ($platform)..."
echo "Moon files : "
for f in `find data -name "*.moon"`; do
  $mcompiler $f
  if [[ $? -ne 0 ]]; then
    got_error=true
  fi
done

echo "C files : "
for f in `find src -name "*.c"`; do
  $compiler -c $cflags $f -o "${f//\//_}.o"
  if [[ $? -ne 0 ]]; then
    got_error=true
  fi
done

echo "Cpp files : "
for f in `find src -name "*.cpp"`; do
  $compiler -c $cflags $f -o "${f//\//_}.o"
  if [[ $? -ne 0 ]]; then
    got_error=true
  fi
done

if [[ ! $got_error ]]; then
  echo "linking..."
  $compiler *.o $lflags
fi

echo "cleaning up..."
# cleaning after cpp
rm *.o
rm res.res 2>/dev/null
# cleaning after moon
echo "done"