function(scint_enable_tracy target)
    CPMAddPackage(
            NAME tracy
            GITHUB_REPOSITORY wolfpld/tracy
            VERSION 0.13.0
    )

    target_include_directories(${target}
            PUBLIC "${Tracy_SOURCE_DIR}/public"
    )

    # Enable Tracy everywhere except Release; macros become no-ops when disabled.
    set(tracy_enabled_configs "$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>")
    set(tracy_disabled_configs "$<NOT:${tracy_enabled_configs}>")

    target_compile_definitions(${target}
            PUBLIC $<${tracy_enabled_configs}:TRACY_ENABLE>
    )

    target_link_libraries(${target}
            PRIVATE $<${tracy_enabled_configs}:TracyClient>
    )

    # Build two header units (enabled/disabled) so we can switch per-config.
    set(tracy_header "${Tracy_SOURCE_DIR}/public/tracy/Tracy.hpp")
    set(tracy_pcm_dir "${CMAKE_BINARY_DIR}/tracy")
    set(tracy_pcm_enabled "${tracy_pcm_dir}/Tracy.hpp.enabled.pcm")
    set(tracy_pcm_disabled "${tracy_pcm_dir}/Tracy.hpp.disabled.pcm")

    file(MAKE_DIRECTORY "${tracy_pcm_dir}")

    message(STATUS "Building Tracy.hpp header unit (enabled) at ${tracy_pcm_enabled}")

    execute_process(
            COMMAND "${CMAKE_CXX_COMPILER}"
            -std=c++23
            -stdlib=libc++
            -fmodule-header
            -DTRACY_ENABLE
            -I"${Tracy_SOURCE_DIR}/public"
            "${tracy_header}"
            -o "${tracy_pcm_enabled}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            RESULT_VARIABLE tracy_hu_enabled_result
            OUTPUT_VARIABLE tracy_hu_enabled_out
            ERROR_VARIABLE  tracy_hu_enabled_err
    )

    if (NOT tracy_hu_enabled_result EQUAL 0)
        message(FATAL_ERROR
                "Failed to build Tracy.hpp (enabled) header unit:\n"
                "${tracy_hu_enabled_out}\n${tracy_hu_enabled_err}"
        )
    endif()

    message(STATUS "Building Tracy.hpp header unit (disabled) at ${tracy_pcm_disabled}")

    execute_process(
            COMMAND "${CMAKE_CXX_COMPILER}"
            -std=c++23
            -stdlib=libc++
            -fmodule-header
            -I"${Tracy_SOURCE_DIR}/public"
            "${tracy_header}"
            -o "${tracy_pcm_disabled}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            RESULT_VARIABLE tracy_hu_disabled_result
            OUTPUT_VARIABLE tracy_hu_disabled_out
            ERROR_VARIABLE  tracy_hu_disabled_err
    )

    if (NOT tracy_hu_disabled_result EQUAL 0)
        message(FATAL_ERROR
                "Failed to build Tracy.hpp (disabled) header unit:\n"
                "${tracy_hu_disabled_out}\n${tracy_hu_disabled_err}"
        )
    endif()

    target_compile_options(${target} PRIVATE
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,${tracy_enabled_configs}>:-fmodule-file=${tracy_pcm_enabled}>
            $<$<AND:$<COMPILE_LANGUAGE:CXX>,${tracy_disabled_configs}>:-fmodule-file=${tracy_pcm_disabled}>
    )
endfunction()
