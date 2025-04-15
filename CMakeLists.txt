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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -std=c++20")

set(SOURCES
    src/main.cpp
    src/io.cpp
    src/parsing_line.cpp
    src/Solver_Lagrange_1D.cpp
)

HEADER_DIRECTORIES(header_dir_list)
list(LENGTH header_dir_list header_dir_list_count)
message(STATUS "[INFO] Found ${header_dir_list_count} header directories")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${header_dir_list}
)
