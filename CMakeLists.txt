cmake_minimum_required(VERSION 3.2.2)

add_subdirectory("Core")
add_subdirectory("Launcher")

# These two are just for testing plug-in linking
add_subdirectory("HelloWorld")
add_subdirectory("APIDepTest")

# Basic utilities
add_subdirectory("SelfTest")
add_subdirectory("Logger")

# Generic Base Layer
add_subdirectory("ModelBase")
add_subdirectory("FilePersistence")
add_subdirectory("VisualizationBase")
add_subdirectory("InteractionBase")
