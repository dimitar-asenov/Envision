cmake_minimum_required(VERSION 3.7.2)

# Prevent in-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds can be messy and therefore disabled. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

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
add_subdirectory("VersionControlUI")

# Object-Oriented layer - Base
add_subdirectory("OOModel")
add_subdirectory("OOVisualization")
add_subdirectory("OOInteraction")

# Object-Oriented layer - Extras
add_subdirectory("CodeReview")
add_subdirectory("JavaExport")
add_subdirectory("OODebug") # uses JavaExport
add_subdirectory("CppExport")
#add_subdirectory("CppImport")
add_subdirectory("CppImportLight")

#add_subdirectory("PythonWrapperGenerator")
#add_subdirectory("InformationScripting") # uses the generator and OODebug

# Object-Oriented layer - Contracts
add_subdirectory("ContractsLibrary")
#add_subdirectory("Alloy")

# Object-Oriented layer - Demos
add_subdirectory("CustomMethodCall")
add_subdirectory("ControlFlowVisualization")
