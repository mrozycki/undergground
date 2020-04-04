add_library(undergground_grader
    compiler.cpp
    grader.cpp
    io_handler.cpp
    test_loader.cpp)

set_project_warnings(undergground_grader)

target_include_directories(undergground_grader
    PUBLIC
        include)

target_link_libraries(undergground_grader
    PUBLIC
        CONAN_PKG::boost
        CONAN_PKG::fmt
        CONAN_PKG::spdlog
    PRIVATE
        undergground_system)

target_compile_features(undergground_grader
    PUBLIC
        cxx_std_17)