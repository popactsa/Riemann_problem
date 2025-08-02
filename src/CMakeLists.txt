file(GLOB_RECURSE SOURCES_CXX LIST_DIRECTORIES true *.hpp *.cpp)
list(REMOVE_ITEM SOURCES_CXX ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
file(GLOB_RECURSE SOURCES_C LIST_DIRECTORIES true *.h *.c)
set(SOURCES ${SOURCES_CXX} ${SOURCES_C})
add_executable(${PROJECT_NAME}_run main.cpp)
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME}_lib PRIVATE
    yaml-cpp
    ${ARMADILLO_LIBRARIES}
    ${SUPERLU_LIBRARIES}
)
target_link_libraries(${PROJECT_NAME}_run PRIVATE
    ${PROJECT_NAME}_lib
)
function(add_common_flags target)
    target_compile_options(${target} PRIVATE
        "-Wall"
        "-Werror"
        "-Wextra"
        "-pedantic"
        "-march=native"
        "-fdiagnostics-color=always"
        "-DARMA_DONT_USE_WRAPPER"
        "-DARMA_USE_SUPERLU"
        "$<$<CONFIG:Release>:-O2>"
        "$<$<CONFIG:Debug>:-O0;-g;-fsanitize=address;-DNDEBUG>"
    )
    target_link_options(${target} PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address>
    )
endfunction()
add_common_flags(${PROJECT_NAME}_run)
add_common_flags(${PROJECT_NAME}_lib)
