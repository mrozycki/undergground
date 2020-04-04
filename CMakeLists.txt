cmake_minimum_required(VERSION 3.10)

project(Undergground CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CONAN_EXTRA_REQUIRES "mysql-connector-c/6.1.11@bincrafters/stable")

include(cmake/CompilerWarnings.cmake)
include(cmake/Conan.cmake)
run_conan()

add_subdirectory(src)