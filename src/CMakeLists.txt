add_executable(undergground
    main.cpp)

target_link_libraries(undergground ${CONAN_LIBS})

add_executable(grader
    grader.cpp)