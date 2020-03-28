add_executable(undergground
    main.cpp
    grader.cpp
    compiler.cpp)

target_link_libraries(undergground PUBLIC
    CONAN_PKG::fmt
    CONAN_PKG::spdlog
    CONAN_PKG::mysql-connector-c)

set_target_properties(undergground PROPERTIES
    CXX_STANDARD 17)