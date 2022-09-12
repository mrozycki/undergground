add_library(undergground_system
    process.cpp)

set_project_warnings(undergground_system)

target_include_directories(undergground_system
    PUBLIC
        include)

target_link_libraries(undergground_system
    PUBLIC
        CONAN_PKG::boost
        CONAN_PKG::fmt
        CONAN_PKG::spdlog)

target_compile_features(undergground_system
    PUBLIC
        cxx_std_17)