function(scint_enable_tracy target)
    # Fetch Tracy and expose its public headers/definitions to the target.
    CPMAddPackage(
            NAME tracy
            GITHUB_REPOSITORY wolfpld/tracy
            VERSION 0.13.0
    )

    target_include_directories(${target}
            PUBLIC "${Tracy_SOURCE_DIR}/public"
    )

    target_compile_definitions(${target}
            PUBLIC TRACY_ENABLE
    )

    target_link_libraries(${target} PRIVATE TracyClient)

    # Build the Tracy.hpp header unit for Clang; fragile but kept isolated here.
    set(tracy_header "${Tracy_SOURCE_DIR}/public/tracy/Tracy.hpp")
    set(tracy_pcm    "${CMAKE_BINARY_DIR}/tracy/Tracy.hpp.pcm")

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tracy")

    message(STATUS "Building Tracy.hpp header unit for Clang at ${tracy_pcm}")

    execute_process(
            COMMAND "${CMAKE_CXX_COMPILER}"
            -std=c++23
            -stdlib=libc++
            -fmodule-header
            -DTRACY_ENABLE
            -I"${Tracy_SOURCE_DIR}/public"
            "${tracy_header}"
            -o "${tracy_pcm}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            RESULT_VARIABLE tracy_hu_result
            OUTPUT_VARIABLE tracy_hu_out
            ERROR_VARIABLE  tracy_hu_err
    )

    if (NOT tracy_hu_result EQUAL 0)
        message(FATAL_ERROR
                "Failed to build Tracy.hpp header unit:\n"
                "${tracy_hu_out}\n${tracy_hu_err}"
        )
    endif()

    target_compile_options(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:-fmodule-file=${tracy_pcm}>
    )
endfunction()
