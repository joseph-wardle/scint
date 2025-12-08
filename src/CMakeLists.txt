add_library(scint STATIC)

target_compile_features(scint PUBLIC cxx_std_23)

target_compile_options(scint PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)


scint_enable_tracy(scint)

target_link_libraries(scint PRIVATE
    vulkan_cppm
)

add_subdirectory(core)
