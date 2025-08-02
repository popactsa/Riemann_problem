cmake_minimum_required(VERSION 3.10...3.15)
project(chlorum)

############# Compiler settings ##############
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# # disable standard flags
# set(CMAKE_C_FLAGS "")
# set(CMAKE_CXX_FLAGS "")
# set(CMAKE_C_FLAGS_DEBUG "")
# set(CMAKE_CXX_FLAGS_DEBUG "")
# set(CMAKE_C_FLAGS_RELEASE "")
# set(CMAKE_CXX_FLAGS_RELEASE "")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_program(CLANGXX_FOUND NAMES "clang++")
find_program(GXX_FOUND NAMES "g++")
if(CLANGXX_FOUND)
    set(CMAKE_CXX_COMPILER "clang++")
elseif(GXX_FOUND)
    set(CMAKE_CXX_COMPILER "g++")
endif()

find_program(CLANG_FOUND NAMES "clang")
find_program(GCC_FOUND NAMES "gcc")
if(CLANG_FOUND)
    set(CMAKE_C_COMPILER "clang")
elseif(GCC_FOUND)
    set(CMAKE_C_COMPILER "gcc")
endif()

# set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

find_package(yaml-cpp REQUIRED)
find_package(SuperLU QUIET)
find_package(Armadillo REQUIRED)

include_directories(src)
add_subdirectory(src)
add_subdirectory(tests)

############# LSP support ##############
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
)
add_custom_target(copy_compile_commands ALL DEPENDS ${CMAKE_SOURCE_DIR}/compile_commands.json)

############# Clang-format ##############
find_program(CLANG_FORMAT_EXE NAMES "clang-format")
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE CODEBASE
        src/*.cpp src/*.hpp src/*.h
        tests/*.cpp tests/*.hpp tests/*.h
    )
    add_custom_target(
        apply_clang-format
        COMMAND ${CLANG_FORMAT_EXE} -style=file -i ${CODEBASE}
        COMMENT "Applying clang-format..."
    )
    if(TARGET ${PROJECT_NAME}_run)
        add_dependencies(${PROJECT_NAME}_run apply_clang-format)
    endif()
    if(TARGET ${PROJECT_NAME}_lib)
        add_dependencies(${PROJECT_NAME}_lib apply_clang-format)
    endif()
    if(TARGET ${PROJECT_NAME}_tests)
        add_dependencies(${PROJECT_NAME}_tests apply_clang-format)
    endif()
else()
    message(WARNING "clang-format not found!")
endif()

