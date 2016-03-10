#!/bin/bash

THIS_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ ! -f "$THIS_SCRIPT_DIR/vis.js" ]; then
  wget "https://cdnjs.cloudflare.com/ajax/libs/vis/4.15.1/vis.min.js" -O vis.js
fi

if [ ! -f "$THIS_SCRIPT_DIR/vis.css" ]; then
  wget "https://cdnjs.cloudflare.com/ajax/libs/vis/4.15.1/vis.min.css" -O vis.css
fi