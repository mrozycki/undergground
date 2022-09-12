add_executable(yaml_converter yaml_converter.cpp)

target_compile_features(yaml_converter
    PUBLIC cxx_std_17
)
