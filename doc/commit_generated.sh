#!/bin/bash

if [ $# -ne 1 ]
then
  echo "Usage: `basename $0` \"Some commit message\""
  exit 1
fi

cd generated
git add -A
git commit -m "$1"
git push origin gh-pages
cd ..