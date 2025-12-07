CPMAddPackage(
        NAME tracy
        GITHUB_REPOSITORY wolfpld/tracy
        VERSION 0.13.0
)


add_library(scint STATIC)

target_compile_features(scint PUBLIC cxx_std_23)

target_compile_options(scint PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(scint
        PUBLIC "${Tracy_SOURCE_DIR}/public"
)

target_compile_definitions(scint
        PUBLIC TRACY_ENABLE
)

target_link_libraries(scint PRIVATE TracyClient)

set(TRACY_HEADER "${Tracy_SOURCE_DIR}/public/tracy/Tracy.hpp")
set(TRACY_PCM    "${CMAKE_BINARY_DIR}/tracy/Tracy.hpp.pcm")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tracy")

message(STATUS "Building Tracy.hpp header unit for Clang at ${TRACY_PCM}")

execute_process(
        COMMAND "${CMAKE_CXX_COMPILER}"
        -std=c++23
        -stdlib=libc++
        -fmodule-header        # build a C++20 header unit
        -DTRACY_ENABLE
        -I"${Tracy_SOURCE_DIR}/public"
        "${TRACY_HEADER}"
        -o "${TRACY_PCM}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        RESULT_VARIABLE TRACY_HU_RESULT
        OUTPUT_VARIABLE TRACY_HU_OUT
        ERROR_VARIABLE  TRACY_HU_ERR
)

if (NOT TRACY_HU_RESULT EQUAL 0)
    message(FATAL_ERROR
            "Failed to build Tracy.hpp header unit:\n"
            "${TRACY_HU_OUT}\n${TRACY_HU_ERR}"
    )
endif()

# Tell clang to load this BMI for header-unit imports of Tracy.hpp.
# `-fmodule-file=[<name>=]<file>` — with name omitted it just loads the file.
target_compile_options(scint PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-fmodule-file=${TRACY_PCM}>
)

add_subdirectory(core)
