#------------------------------------------------------------------------------#
# CMakeLists.txt
# Rémi Attab (remi.attab@gmail.com), 30 Dec 2012
# FreeBSD-style copyright and disclaimer apply
#------------------------------------------------------------------------------#


#------------------------------------------------------------------------------#
# CONFIG
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 2.6)
project(reflect)

enable_testing()

add_definitions("-Wall -Wextra -Werror -pedantic -g -O4")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


#------------------------------------------------------------------------------#
# COMPILER TESTS
#------------------------------------------------------------------------------#

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-std=c++11 REFLECT_FLAG_C11)
check_cxx_compiler_flag(-std=c++0x REFLECT_FLAG_C0X)

if (REFLECT_FLAG_C11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (REFLECT_FLAG_C0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()


#------------------------------------------------------------------------------#
# ENV TEST
#------------------------------------------------------------------------------#

include_directories("includes")

include_directories("$ENV{HOME}/local/include")
link_directories("$ENV{HOME}/local/lib")


#------------------------------------------------------------------------------#
# LIBRARIES
#------------------------------------------------------------------------------#

link_libraries(rt)
link_libraries(pthread)

option(USE_TCMALLOC "Use tcmalloc for heap allocations." ON)
find_library(LIB_TCMALLOC tcmalloc)

if(USE_TCMALLOC)
    if(LIB_TCMALLOC MATCHES "NOTFOUND")
        message("WARNING: unable to find tcmalloc")
    else()
        link_libraries(tcmalloc)
        add_definitions("-fno-builtin-malloc -fno-builtin-calloc")
        add_definitions("-fno-builtin-realloc -fno-builtin-free")
    endif()
endif()


#------------------------------------------------------------------------------#
# BUILD
#------------------------------------------------------------------------------#

include_directories("${PROJECT_SOURCE_DIR}/src")

install(
    FILES
    src/argument.h
    src/argument.tcc
    src/cast.h
    src/function.h
    src/function.tcc
    src/function_traits.h
    src/reflect.h
    src/ref_type.h
    src/registry.h
    src/type.h
    src/type.tcc
    src/type_name.h
    src/type_vector.h
    src/utils.h
    src/value_function.h
    src/value.h
    src/value.tcc

    DESTINATION
    include/reflect)

install(
    FILES
    src/types/void.h
    src/types/int.h
    src/types/primitives.h
    src/types/value.h

    DESTINATION
    include/reflect/types)

install(
    FILES
    src/reflect/basics.h
    src/reflect/constructors.h
    src/reflect/class.h
    src/reflect/field.h
    src/reflect/function.h
    src/reflect/names.h

    DESTINATION
    include/reflect/reflect)

add_library(
    reflect
    SHARED
    src/reflect.cpp)

install(TARGETS reflect DESTINATION lib)


#------------------------------------------------------------------------------#
# TESTS
#------------------------------------------------------------------------------#

function(reflect_test name)
    if(CMAKE_SOURCE_DIR STREQUAL ${PROJECT_SOURCE_DIR})
        add_executable(${name}_test tests/${name}_test.cpp)
        target_link_libraries(${name}_test reflect boost_unit_test_framework)
        add_test(${name} bin/${name}_test)
    endif()
endfunction()

reflect_test(ref)
reflect_test(value)
reflect_test(value_function)
reflect_test(function)
reflect_test(reflection)
