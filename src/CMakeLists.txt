add_executable(undergground
    grader/compiler.cpp
    grader/grader.cpp
    grader/io_handler.cpp
    grader/test_loader.cpp
    system/process.cpp
    database.cpp
    main.cpp
)

target_link_libraries(undergground PUBLIC
    CONAN_PKG::fmt
    CONAN_PKG::spdlog
    CONAN_PKG::mysql-connector-c
    CONAN_PKG::boost
)

target_include_directories(undergground PRIVATE .)

target_compile_features(undergground
    PUBLIC cxx_std_17
)