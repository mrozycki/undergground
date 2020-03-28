add_executable(undergground
    main.cpp
    grader.cpp)

target_link_libraries(undergground ${CONAN_LIBS})