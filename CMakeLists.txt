cmake_minimum_required(VERSION 3.2.2)

add_subdirectory("Core")
add_subdirectory("Launcher")

# These two are just for testing plug-in linking
add_subdirectory("HelloWorld")
add_subdirectory("APIDepTest")