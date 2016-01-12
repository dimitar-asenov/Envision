#!/usr/bin/python2

import os
import shutil

envisionDirectory = '/home/patrick/Envision/'

def copy(file):
	filePath = envisionDirectory + 'DebugBuild/cpp_export/' + file
	if not os.path.exists(os.path.dirname(filePath)):
		os.makedirs(os.path.dirname(filePath))
	shutil.copyfile(envisionDirectory + file, filePath)


def cMakeListsAndPrecompiled(name):
	copy(name + '/CMakeLists.txt')
	copy(name + '/src/precompiled.h')

def corePlugin(name):
	cMakeListsAndPrecompiled(name)
	copy(name + '/src/' + name.lower() + '_api.h')

def plugin(name):
	corePlugin(name)
	copy(name + '/' + name.lower() + '.plugin')

def createCMakeLists(lines):
	f = open(envisionDirectory + 'DebugBuild/cpp_export/CMakeLists.txt','w+')
	f.write('cmake_minimum_required(VERSION 3.2.2)\n\n')
	for line in lines:
		f.write('add_subdirectory(\"' + line + '\")\n')
	f.close()

def createCommonCmake():
	if not os.path.exists(envisionDirectory + 'DebugBuild/cpp_export/CMake'):
		os.makedirs(envisionDirectory + 'DebugBuild/cpp_export/CMake')
	with open(envisionDirectory + 'CMake/common.cmake') as f:
		lines = f.readlines()
	f = open(envisionDirectory + 'DebugBuild/cpp_export/CMake/common.cmake','w+')
	skipLine = False
	for item in lines:
		if item.strip().startswith('add_custom_command'):
			skipLine = True
		if not skipLine:
			f.write(item)
		elif item.strip() == ')':
			skipLine = False
	f.close()


corePlugin('Core')
copy('Core/src/reflect/typeIdMacros.h')
cMakeListsAndPrecompiled('Launcher')
plugin('Logger')
plugin('SelfTest')

createCMakeLists(['Core', 'Launcher', 'Logger'])
createCommonCmake()

