add_executable(undergground
    system/process.cpp
    compiler.cpp
    database.cpp
    grader.cpp
    main.cpp
)

target_link_libraries(undergground PUBLIC
    CONAN_PKG::fmt
    CONAN_PKG::spdlog
    CONAN_PKG::mysql-connector-c
    CONAN_PKG::boost
)

target_compile_features(undergground
    PUBLIC cxx_std_17
)