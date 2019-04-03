#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo "Script Directory: $DIR"
contract=""

while [ $# -gt 0 ]; do

   if [[ $1 == *"--"* ]]; then
        echo "$1 | $2"
        v="${1/--/}"
        declare $v="$2"
   fi

  shift
done

mkdir -p build
cd build
if [[ $contract == "" ]];
     then
          for filename in $DIR/src/*.cpp; do
               echo "Compiling $filename"
               eosio-cpp $filename -abigen
          done
     else
          echo "Compiling ${contract//\\//}"
          eosio-cpp $DIR/${contract//\\//} -abigen
fi