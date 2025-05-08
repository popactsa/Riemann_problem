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

# find_package(SuperLU QUIET)
# if(NOT SuperLU_FOUND)
#     find_path(SUPERLU_INCLUDE_DIR NAMES supermatrix.h PATHS /usr/include/superlu)
#     find_library(SUPERLU_LIBRARY NAMES superlu PATHS /usr/lib /usr/local/lib)
#
#     if(SUPERLU_INCLUDE_DIR AND SUPERLU_LIBRARY)
#         set(SuperLU_FOUND TRUE)
#         set(SuperLU_INCLUDE_DIRS ${SUPERLU_INCLUDE_DIR})
#         set(SuperLU_LIBRARIES ${SUPERLU_LIBRARY})
#         execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold
#             "[INFO] SuperLU found!")
#     else()
#         message(FATAL_ERROR "SuperLU not found")
#     endif()
# endif()
# find_package(Armadillo REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -Wextra -Werror -pedantic -std=c++20")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -lsuperlu -Wextra -Werror -pedantic -std=c++20 -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -DARMA_DONT_USE_WRAPPER -DARMA_USE_SUPERLU -lopenblas -lsuperlu -Wextra -Werror -pedantic -std=c++20 -march=native")

set(SOURCES
    src/main.cpp
    src/io.cpp
    src/parsing_line.cpp
    src/Parser.cpp
    src/Solver_Lagrange_1D.cpp
    # src/test.cpp
)

HEADER_DIRECTORIES(header_dir_list)
list(LENGTH header_dir_list header_dir_list_count)
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold
    "[INFO] Found ${header_dir_list_count} headers")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${ARMADILLO_INCLUDE_DIRS}
    ${SUPERLU_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${ARMADILLO_LIBRARIES}
    ${SUPERLU_LIBRARIES}
)
if(ARMADILLO_FOUND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ARMA_USE_SUPERLU)
endif()
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${header_dir_list}
)
# set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_CXX_FLAGS "-save-temps")
