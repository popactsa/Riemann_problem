cmake_minimum_required(VERSION 3.10...3.15)
project(Riemann)

# Compiler settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -DARMA_DONT_USE_WRAPPER -DARMA_USE_SUPERLU -lopenblas -lsuperlu -Wextra -Werror -pedantic -std=c++20 -march=native")

# Main project sources
set(SOURCES
    src/io.cpp
    src/parsing_line.cpp
    src/Parser.cpp
    src/Solver_Lagrange_1D.cpp
    src/Solver_Godunov_1D.cpp
)

# Macro to find all header directories
macro(HEADER_DIRECTORIES return_list)
    file(GLOB_RECURSE new_list 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.hpp"
    )
    set(dir_list "")
    foreach(file_path ${new_list})
        get_filename_component(dir_path ${file_path} DIRECTORY)
        set(dir_list ${dir_list} ${dir_path})
    endforeach()
    list(REMOVE_DUPLICATES dir_list)
    set(${return_list} ${dir_list})
endmacro()

HEADER_DIRECTORIES(header_dir_list)
list(LENGTH header_dir_list header_dir_list_count)
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue --bold
    "[INFO] Found ${header_dir_list_count} headers")

add_library(project_objects OBJECT ${SOURCES})

target_include_directories(project_objects PRIVATE
    ${header_dir_list}
)

target_include_directories(project_objects SYSTEM PRIVATE
    ${ARMADILLO_INCLUDE_DIRS}
    ${SUPERLU_INCLUDE_DIRS}
)

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp $<TARGET_OBJECTS:project_objects>)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${header_dir_list}
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${ARMADILLO_INCLUDE_DIRS}
    ${SUPERLU_INCLUDE_DIRS}
    ${googletest_SOURCE_DIR}/googletest/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${ARMADILLO_LIBRARIES}
    ${SUPERLU_LIBRARIES}
)

if(ARMADILLO_FOUND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ARMA_USE_SUPERLU)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Creating symlink for LSP"
)

# Tests configuration
enable_testing()

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
    tests/parser_test.cpp
    tests/parser_unittest.cpp
)

add_executable(
    tests
    ${TEST_SOURCES}
    $<TARGET_OBJECTS:project_objects>
)

target_include_directories(tests PRIVATE
    ${header_dir_list}
)

target_include_directories(tests SYSTEM PRIVATE
    ${ARMADILLO_INCLUDE_DIRS}
    ${SUPERLU_INCLUDE_DIRS}
    ${googletest_SOURCE_DIR}/googletest/include
)


target_link_libraries(
    tests
  GTest::gtest_main
  ${ARMADILLO_LIBRARIES}
  ${SUPERLU_LIBRARIES}
)
target_compile_options(tests PRIVATE
    -DUNIT_TESTING
)

include(GoogleTest)
gtest_discover_tests(tests)
