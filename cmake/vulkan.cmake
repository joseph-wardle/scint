find_package(Vulkan REQUIRED)

# This must be a real library, not INTERFACE
add_library(vulkan_cppm STATIC) # or OBJECT, SHARED – STATIC is fine

target_sources(vulkan_cppm
        PUBLIC
        FILE_SET cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS "${Vulkan_INCLUDE_DIR}"          # or ${Vulkan_INCLUDE_DIRS}
        FILES      "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
)

target_link_libraries(vulkan_cppm
        PUBLIC Vulkan::Vulkan
)
