#!/bin/bash

cflags="-Wall -03 -g -std=gnu11 -fno-strict-aliasing -Isrc"
lflags="-lSDL2 -lm"


platform="unix"
out="scuffTXT"
compiler="gcc"
mcompiler="moonc"
cflags="$cflags -DLUA_USE_POSIX"
lflags="$lflags -o $out"



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

if [[ ! $got_error ]]; then
  echo "linking..."
  $compiler *.o $lflags
fi

echo "cleaning up..."
#TODO
echo "done"