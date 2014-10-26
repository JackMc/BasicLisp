cmake_minimum_required (VERSION 2.6)
project (basic_lisp)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set (Lisp_SRCS
    ${PROJECT_SOURCE_DIR}/builtins_base.c
    ${PROJECT_SOURCE_DIR}/eval.c
    ${PROJECT_SOURCE_DIR}/lisp.c
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/print.c
    ${PROJECT_SOURCE_DIR}/reader.c
    ${PROJECT_SOURCE_DIR}/builtins_math.c
    ${PROJECT_SOURCE_DIR}/builtins_interpreter.c)

# Compiler options
add_definitions (-g -Wall)

include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_INCLUDE_DIR}")

add_executable (${PROJECT_NAME} ${Lisp_SRCS})

target_link_libraries(${PROJECT_NAME} m)