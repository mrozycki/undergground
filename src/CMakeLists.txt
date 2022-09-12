add_subdirectory(system)
add_subdirectory(grader)

add_executable(undergground
    database.cpp
    main.cpp
)

set_project_warnings(undergground)

target_link_libraries(undergground
    PRIVATE
        undergground_grader
        undergground_system
        CONAN_PKG::fmt
        CONAN_PKG::spdlog
        CONAN_PKG::mysql-connector-c
)

target_include_directories(undergground PRIVATE .)

target_compile_features(undergground
    PUBLIC cxx_std_17
)

add_subdirectory(tools)
