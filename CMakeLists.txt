cmake_minimum_required(VERSION 3.8)
project(spritorama)

file(TO_CMAKE_PATH "${CMAKE_HOME_DIRECTORY}/bin" BinaryOutputDir)
file(MAKE_DIRECTORY ${BinaryOutputDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinaryOutputDir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BinaryOutputDir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BinaryOutputDir})

# needed so Visual Studio does not create configuration sub-directories
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} ${BinaryOutputDir})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config} ${BinaryOutputDir})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config} ${BinaryOutputDir})
endforeach(config)

set(DEFAULT_COMPILE_OPTIONS_DEBUG
    $<$<PLATFORM_ID:Windows>:/std:c++17>
    $<$<PLATFORM_ID:Linux>:-Wall>
    $<$<PLATFORM_ID:Linux>:-Wcast-qual>             # trying to change const qualifier when casting pointers
    $<$<PLATFORM_ID:Linux>:-Wconversion>            # implicit conversions between int/real and signed/unsigned
    $<$<PLATFORM_ID:Linux>:-Wdouble-promotion>      # float is promoted to double
    $<$<PLATFORM_ID:Linux>:-Werror>
    $<$<PLATFORM_ID:Linux>:-Wextra>
    $<$<PLATFORM_ID:Linux>:-Wnull-dereference>
    $<$<PLATFORM_ID:Linux>:-Wold-style-cast>
    $<$<PLATFORM_ID:Linux>:-Wpedantic -pedantic-errors>
    $<$<PLATFORM_ID:Linux>:-Wshadow>
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-branches> # duplicated branch in after if/else
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>     # duplicated condition in if/else
    $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>          # misuse of logical operators and duplicate operands
    $<$<CXX_COMPILER_ID:GNU>:-Wuseless-cast>)       # cast to same type as self
set(DEFAULT_COMPILE_OPTIONS_RELEASE ${DEFAULT_COMPILE_OPTIONS_DEBUG})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(THIRDPARTY_ROOT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/src/thirdparty")

# needs to be set here so it can be used by the sub project
set(LAUNCHER_NAME "spritorama")

add_subdirectory(src/spritorama)

# this needs to be set in the root CMakeLists.txt to the name of the executable
# as it is called by add_executable
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
    VS_STARTUP_PROJECT ${LAUNCHER_NAME})
