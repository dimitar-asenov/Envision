#!/bin/bash

#specify the cores with just as with make


for dir in ./*/
do
  (cd $dir && make debug-install $1)
  echo "Processed $dir"
done
