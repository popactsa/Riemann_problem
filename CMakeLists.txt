cmake_minimum_required(VERSION 3.10...3.15)
project(Riemann)

macro(HEADER_DIRECTORIES return_list)
    # Macro to recursively add all header files
    file(GLOB_RECURSE new_list *.h)
    set(dir_list "")
    foreach(file_path ${new_list})
        get_filename_component(dir_path ${file_path} DIRECTORY)
        set(dir_list ${dir_list} ${dir_path})
    endforeach()
    list(REMOVE_DUPLICATES dir_list)
    set(${return_list} ${dir_list})
endmacro()

# set(CMAKE_OBJECT_PATH_DIR "${CMAKE_BINARY_DIR}/object_files")
# set(CMAKE_CLEAN_NO_CUSTOM 1)
# set(CMAKE_KEEP_TEMP_FILES ON)
# # set(CMAKE_VERBOSE_MAKEFILE ON)
# file(MAKE_DIRECTORY ${CMAKE_OBJECT_PATH_DIR})
#
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_OBJECT_PATH_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_OBJECT_PATH_DIR})
# set(CMAKE_OBJECT_OUTPUT_DIRECTORY ${CMAKE_OBJECT_PATH_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -Wextra -Werror -pedantic -std=c++20")

set(SOURCES
    src/main.cpp
    src/io.cpp
    src/parsing_line.cpp
    src/Solver_Lagrange_1D.cpp
    # src/test.cpp
)

HEADER_DIRECTORIES(header_dir_list)
list(LENGTH header_dir_list header_dir_list_count)
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold
    "[INFO] Found ${header_dir_list_count} headers")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${header_dir_list}
)
# set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_CXX_FLAGS "-save-temps")
