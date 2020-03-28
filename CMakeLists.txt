cmake_minimum_required(VERSION 3.10)

project(Undergground)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

include(build/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)