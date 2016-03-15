cmake_minimum_required(VERSION 3.2.2)

add_subdirectory("Core")
add_subdirectory("Launcher")

# These two are just for testing plug-in linking
add_subdirectory("HelloWorld")
add_subdirectory("APIDepTest")

# Basic utilities
add_subdirectory("SelfTest")
add_subdirectory("Logger")

# Generic Layer - Base
add_subdirectory("ModelBase")
add_subdirectory("FilePersistence")
add_subdirectory("VisualizationBase")
add_subdirectory("InteractionBase")

# Generic Layer - Extras
add_subdirectory("Comments")
add_subdirectory("Export")

# Object-Oriented layer - Base
add_subdirectory("OOModel")
add_subdirectory("OOVisualization")
add_subdirectory("OOInteraction")

# Object-Oriented layer - Extras
add_subdirectory("CodeReview")
add_subdirectory("JavaExport")
add_subdirectory("OODebug") # uses JavaExport
add_subdirectory("CppExport")
add_subdirectory("CppImport")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# This project uses C++11 ABI tags which were introduced with GCC 5 and are currently not
	# supported by Clang.
	add_subdirectory("PythonWrapperGenerator")
endif()
add_subdirectory("InformationScripting") # uses the generator and OODebug

# Object-Oriented layer - Contracts
add_subdirectory("ContractsLibrary")
add_subdirectory("Alloy")

# Object-Oriented layer - Demos
add_subdirectory("CustomMethodCall")
add_subdirectory("ControlFlowVisualization")
