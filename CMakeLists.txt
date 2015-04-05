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

# For cperf optimization only.
# add_definitions("-Wno-unused-variable -Wno-unused-parameter")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#------------------------------------------------------------------------------#
# UTILS
#------------------------------------------------------------------------------#

# Forces the library to be linked in. Useful when the library consist only of
# static constructors.
function(force_target_link_libraries target lib)
    target_link_libraries(${target} -Wl,--no-as-needed ${lib} -Wl,--as-needed)
endfunction()


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

add_library(reflect SHARED
    src/reflect.cpp
    src/types/primitive_void.cpp
    src/types/reflect/value.cpp
    src/types/reflect/type.cpp)


add_library(reflect_primitives SHARED
    src/types/primitives.cpp
    src/types/primitive_bool.cpp
    src/types/primitive_char.cpp
    src/types/primitive_short.cpp
    src/types/primitive_int.cpp
    src/types/primitive_long.cpp
    src/types/primitive_long_long.cpp)

add_library(reflect_std SHARED
    src/types/std/string.cpp)

# add_library(reflect_json SHARED src/utils/json/json.cpp)
# target_link_libraries(reflect_json reflect)

# add_library(reflect_config SHARED src/utils/config/compile.cpp)
# target_link_libraries(reflect_config reflect_json)
# force_target_link_libraries(reflect_config reflect_primitives)
# force_target_link_libraries(reflect_config reflect_std)


#------------------------------------------------------------------------------#
# INSTALL
#------------------------------------------------------------------------------#

install(TARGETS reflect DESTINATION lib)

install(
    FILES
    src/argument.h
    src/argument.tcc
    src/cast.h
    src/function.h
    src/function.tcc
    src/scope.h
    src/scope.tcc
    src/overloads.h
    src/overloads.tcc
    src/reflect.h
    src/ref_type.h
    src/registry.h
    src/type.h
    src/type.tcc
    src/type_vector.h
    src/utils.h
    src/value_function.h
    src/value.h
    src/value.tcc
    DESTINATION
    include/reflect)

install(
    FILES
    src/dsl/all.h
    src/dsl/basics.h
    src/dsl/type.h
    src/dsl/field.h
    src/dsl/function.h
    src/dsl/scope.h
    src/dsl/template.h
    src/dsl/operators.h
    src/dsl/plumbing.h
    DESTINATION
    include/reflect/reflect)

install(
    FILES
    src/types/primitives.h
    src/types/pointer.h
    DESTINATION
    include/reflect/types/reflect)

install(
    FILES
    src/types/reflect/value.h
    src/types/reflect/type.h
    DESTINATION
    include/reflect/types/reflect)

install(
    FILES
    src/types/std/smart_ptr.h
    src/types/std/string.h
    src/types/std/vector.h
    DESTINATION
    include/reflect/types/std)


#------------------------------------------------------------------------------#
# TESTS
#------------------------------------------------------------------------------#

add_library(reflect_test SHARED tests/test_types.cpp)
force_target_link_libraries(reflect_test reflect_primitives)

function(reflect_test name)
    if(CMAKE_SOURCE_DIR STREQUAL ${PROJECT_SOURCE_DIR})
        add_executable(${name}_test tests/${name}_test.cpp)
        target_link_libraries(${name}_test reflect)
        force_target_link_libraries(${name}_test reflect_primitives)
        force_target_link_libraries(${name}_test reflect_test)
        force_target_link_libraries(${name}_test reflect_std)
        target_link_libraries(${name}_test boost_unit_test_framework)
        add_test(${name} bin/${name}_test)
    endif()
endfunction()

function(reflect_utils_test utils name)
    if(CMAKE_SOURCE_DIR STREQUAL ${PROJECT_SOURCE_DIR})
        reflect_test(${name})
        target_link_libraries(${name}_test reflect_${utils})
    endif()
endfunction()

reflect_test(ref)
reflect_test(scope)
reflect_test(type)
reflect_test(value)
reflect_test(value_function)
reflect_test(function)
reflect_test(pointer)
reflect_test(reflection)
reflect_test(demo)
# reflect_utils_test(json json)
# reflect_utils_test(config path)
# reflect_utils_test(config cubes)

add_executable(blah_test tests/blah_test.cpp)
target_link_libraries(blah_test reflect)
force_target_link_libraries(blah_test reflect_primitives)
force_target_link_libraries(blah_test reflect_std)


#------------------------------------------------------------------------------#
# CPERF
#------------------------------------------------------------------------------#

function(reflect_cperf name)
    if(CMAKE_SOURCE_DIR STREQUAL ${PROJECT_SOURCE_DIR})
        add_executable(${name}_test tests/cperf/${name}_test.cpp)
        target_link_libraries(${name}_test reflect)
        add_test(${name} bin/${name}_test)
    endif()
endfunction()


reflect_cperf(type_lookup)
reflect_cperf(reflect_args)
reflect_cperf(reflect_getter)
reflect_cperf(reflect_setter)
