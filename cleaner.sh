#!/bin/bash

#specify the cores with just as with make


for dir in ./*/
do
  (cd $dir && make clean)
  echo "Processed $dir"
done
