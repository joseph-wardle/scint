add_library(scint STATIC)

target_compile_features(scint PUBLIC cxx_std_23)

target_compile_options(scint PRIVATE -Wall -Wextra -Wpedantic)

add_subdirectory(core)
