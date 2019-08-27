#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo "Script Directory: $DIR"
#contract=""

# while [ $# -gt 0 ]; do
#      if [[ $1 == *"--"* ]]; then
#           echo "$1 | $2"
#           v="${1/--/}"
#           declare $v="$2"
#      fi
#      shift
# done

for contractDir in $DIR/contracts/*/
do
     cd $contractDir
     contract=${contractDir%*/}
     contract=${contract##*/}
     echo "Building: $contract"
     eosio-cpp -abigen -I include -L /usr/opt/eosio.cdt/1.6.2/include/eosiolib/contracts/eosio -contract $contract -o $contract.wasm src/$contract.cpp
done