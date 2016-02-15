#!/usr/bin/python2
import os, re, shutil

envisionDirectory = os.path.dirname( os.path.dirname( os.path.dirname(os.path.realpath(__file__)))) + '/'

hOrCppRegex = re.compile(r'^.*\.(h|cpp|hpp)$', re.DOTALL)

def copyFileAndCreateDirIfNotExist(src, dst):
	if not os.path.exists(os.path.dirname(dst)):
		os.makedirs(os.path.dirname(dst))
	shutil.copyfile(src, dst)
	
def copyIfExists(file):
	if os.path.exists(envisionDirectory + file):
		copyFileAndCreateDirIfNotExist(envisionDirectory + file, envisionDirectory + 'DebugBuild/cpp_export/' + file)

def copyAllFilesButHAndCpp(src, dst):
	for root, dirs, files in os.walk(src):
		for basename in files:
			if not hOrCppRegex.match(basename):
				filename = os.path.join(root, basename)
				copyFileAndCreateDirIfNotExist(filename, filename.replace(src, dst))

def plugin(name):
	copyIfExists(name + '/src/precompiled.h')
	copyIfExists(name + '/src/' + name.lower() + '_api.h')
	copyAllFilesButHAndCpp(envisionDirectory + name, envisionDirectory + 'DebugBuild/cpp_export/' + name)

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

plugin('Core')
plugin('Launcher')
plugin('HelloWorld')
plugin('APIDepTest')
plugin('Logger')
plugin('SelfTest')
plugin('Logger')
plugin('ModelBase')

createCMakeLists(['Core', 'Launcher', 'HelloWorld', 'APIDepTest', 'SelfTest', 'Logger', 'ModelBase'])
createCommonCmake()

