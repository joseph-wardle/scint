cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_MODULE_STD ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

project(scint LANGUAGES CXX C)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)


include(cmake/get_cpm.cmake)

option(BUILD_SANDBOX "Build sandbox executable" ON)
option(BUILD_TESTS "Build tests" ON)

add_subdirectory(src)

if(BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
