#!/bin/bash
set -e

WORKSPACE_DIR=$( cd $(dirname $0)/.. ; pwd )
QT_SRC_DIR=$WORKSPACE_DIR/qt/qt-envision-enhancements
QT_BUILD_DIR=$WORKSPACE_DIR/qt/qt-build
QT_LIBS=$QT_BUILD_DIR/lib
ENVISION_QT_LIB_LIND_DIR=$WORKSPACE_DIR/DebugBuild/qt

echo "################################################################################"
echo "Cloning Qt repository to" $QT_SRC_DIR
echo "################################################################################"
git clone git://gitorious.org/~mitko/qt/qt-envision-enhancements.git $QT_SRC_DIR

cd $QT_SRC_DIR

echo "################################################################################"
echo "Configuring Qt Build in" $QT_SRC_DIR
echo "Build prefix is" $QT_BUILD_DIR
echo "################################################################################"
./configure -prefix $QT_BUILD_DIR -release -opensource -no-webkit -no-javascript-jit -no-gtkstyle -nomake examples -nomake demos

echo "################################################################################"
echo "Compiling Qt at" $QT_SRC_DIR
echo "################################################################################"
make -j8 | grep "error:"

echo "################################################################################"
echo "Installing Qt to" $QT_BUILD_DIR
echo "################################################################################"
make install

echo "################################################################################"
echo "Creating symbolic link" $ENVISION_QT_LIB_LIND_DIR
echo "################################################################################"
ln -s $QT_LIBS $ENVISION_QT_LIB_LIND_DIR