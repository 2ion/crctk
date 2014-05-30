#!/bin/bash
# this script tests the crctk binary for errors

set -e
set -x

readonly WD=$(mktemp -d)
readonly CRCTK="${PWD}/../src/crctk"
cd "$WD"
echo "working directory is: $WD"

trap cleanup SIGTERM SIGINT EXIT

create_binary_file () {
  local ofile=$1
  dd if=/dev/urandom bs=128 count=1 "of=${ofile}" &>/dev/null || return 1
  return 0
}

cleanup () {
  rm -rf "$WD" || return 1
  return 0
}

# main ()

# some files to work with

create_binary_file "one.file"
create_binary_file "two.file"
create_binary_file "three.file"

# -c
"$CRCTK" -c one.file

# -C
"$CRCTK" -C one.db one.file two.file three.file
"$CRCTK" -V two.db one.file two.file

# -V
"$CRCTK" -V one.db
