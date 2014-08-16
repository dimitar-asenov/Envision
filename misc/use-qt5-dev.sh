#!/bin/bash
set -u
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
WORKSPACE_DIR=$SCRIPT_DIR/..
QT_SRC_DIR=$WORKSPACE_DIR/qt/qt5-dev
QT_BUILD_DIR=$WORKSPACE_DIR/qt/qt5-build
QT_LIBS=$QT_BUILD_DIR/lib
ENVISION_QT_LIB_LINK_DIR=$WORKSPACE_DIR/DebugBuild/qt

echo "################################################################################"
echo "Installing Qt 5 build dependencies"
echo "################################################################################"
sudo apt-get install build-essential perl python git
sudo apt-get install "^libxcb.*" libx11-xcb-dev libglu1-mesa-dev libxrender-dev
sudo apt-get install flex bison gperf libicu-dev libxslt-dev ruby
sudo apt-get install libasound2-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev
sudo apt-get install libfontconfig1-dev

echo "################################################################################"
echo "Cloning Qt repository to" $QT_SRC_DIR
echo "################################################################################"
git clone git://gitorious.org/qt/qt5.git $QT_SRC_DIR
cd $QT_SRC_DIR
git checkout dev

echo "################################################################################"
echo "Initializing submodules"
echo "################################################################################"
./init-repository

echo "################################################################################"
echo "Configuring Qt Build in" $QT_SRC_DIR
echo "Build prefix is" $QT_BUILD_DIR
echo "################################################################################"
./configure -prefix $QT_BUILD_DIR -release -separate-debug-info -force-debug-info -nomake examples -nomake tests -no-gtkstyle -confirm-license -opensource

echo "################################################################################"
echo "Compiling Qt at" $QT_SRC_DIR
echo "################################################################################"
make -j8

echo "################################################################################"
echo "Installing Qt to" $QT_BUILD_DIR
echo "################################################################################"
make install

echo "################################################################################"
echo "Creating symbolic link" $ENVISION_QT_LIB_LINK_DIR
echo "################################################################################"
ln -s $QT_LIBS $ENVISION_QT_LIB_LINK_DIR

echo "################################################################################"
echo "DONE"
echo "################################################################################"