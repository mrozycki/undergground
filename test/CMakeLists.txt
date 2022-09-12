# automatically enable catch2 to generate ctest targets
if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)

add_executable(tests integration.cpp)
target_link_libraries(tests PRIVATE catch_main undergground_grader CONAN_PKG::spdlog)

message("Project source dir: " ${CMAKE_CURRENT_LIST_DIR})

catch_discover_tests(tests
  EXTRA_ARGS
    -s
    --reporter=xml
    --out=tests.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
