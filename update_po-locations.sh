#!/bin/bash

set -e
set -v
set -x

cd po-locations

for file in *.po; 
do 
  ./regen.sh $file;
done;

cd ..
