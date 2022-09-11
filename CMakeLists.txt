cmake_minimum_required(VERSION 3.10)

project(Undergground CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

option(ENABLE_TESTING "Enable Test Builds" ON)

include(cmake/CompilerWarnings.cmake)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(src)

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()