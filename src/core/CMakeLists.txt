set(SCINT_CORE_MODULES
        types.cppm
)

target_sources(scint
    PUBLIC
        FILE_SET core_modules
            TYPE CXX_MODULES
            BASE_DIRS
                ${CMAKE_CURRENT_SOURCE_DIR}
            FILES
                ${SCINT_CORE_MODULES}
)
