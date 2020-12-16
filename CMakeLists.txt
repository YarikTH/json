cmake_minimum_required(VERSION 3.1)

##
## PROJECT
## name and version
##
project(nlohmann_json VERSION 3.9.1 LANGUAGES CXX)

##
## INCLUDE
##
##
include(ExternalProject)

##
## OPTIONS
##

if (POLICY CMP0077)
    # Allow CMake 3.13+ to override options when using FetchContent / add_subdirectory.
    cmake_policy(SET CMP0077 NEW)
endif ()

if( CMAKE_CXX_COMPILER_ID STREQUAL Clang )
    include(cmake/ucm.cmake)
    option(JSON_CPP_STD_LIB "C++ standard library" "libstdc++")
    ucm_add_flags(CXX "-stdlib=${JSON_CPP_STD_LIB}")
    ucm_add_flags(CXX "-ftemplate-backtrace-limit=0")
    ucm_print_flags()
endif()

# set( INCLUDE_PATH_TO_USE ${PROJECT_SOURCE_DIR}/include/ )
set( INCLUDE_PATH_TO_USE ${PROJECT_SOURCE_DIR}/single_include/ )

add_library( test_issue_2491 test_issue_2491.cpp )

target_include_directories( test_issue_2491 PUBLIC ${PROJECT_SOURCE_DIR}/single_include/ )


add_library( test_issue_2491_with_magic test_issue_2491.cpp )

target_include_directories( test_issue_2491_with_magic PUBLIC ${PROJECT_SOURCE_DIR}/single_include/ )

target_compile_definitions( test_issue_2491_with_magic PRIVATE MAGIC )
