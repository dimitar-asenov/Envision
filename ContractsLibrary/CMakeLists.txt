cmake_minimum_required(VERSION 3.7.2)

project(ContractsLibrary)
include(../CMake/common.cmake)

add_library(ContractsLibrary SHARED
	src/items/ContractFilter.h
	src/items/SignatureContractsVMethodAddOn.h
	src/items/InterfaceContractsVMethodAddOn.h
	src/monitor/ValueAtReturnVisitor.h
	src/monitor/ChangeMonitor.h
	src/precompiled.h
	src/ContractsLibraryException.h
	src/contractslibrary_api.h
	src/ContractsLibraryPlugin.h
	src/items/ContractFilter.cpp
	src/items/SignatureContractsVMethodAddOn.cpp
	src/items/InterfaceContractsVMethodAddOn.cpp
	src/monitor/ValueAtReturnVisitor.cpp
	src/monitor/ChangeMonitor.cpp
	src/ContractsLibraryException.cpp
	test/SimpleTest.cpp
	src/ContractsLibraryPlugin.cpp
)

envision_plugin(ContractsLibrary OOInteraction)