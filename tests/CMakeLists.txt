if(CMAKE_BUILD_TYPE MATCHES "Debug|Coverage")
    set(BINARY ${CMAKE_PROJECT_NAME}_tests)
    find_package(GTest REQUIRED)
    add_executable(${PROJECT_NAME}_tests
        main.cpp
        Io_unit_test.cpp
    )

    function(add_common_flags target)
        target_compile_options(${target} PRIVATE
            "-O0"
            "-Wall"
            "-Werror"
            "-Wextra"
            "-pedantic"
            "-march=native"
            "-fdiagnostics-color=always"
        )
        target_link_options(${target} PRIVATE
            "-fsanitize=address"
            # "$<$<CONFIG:Coverage>:-fprofile-instr-generate;-fcoverage-mapping>"
        )
    endfunction()
    add_common_flags(${PROJECT_NAME}_tests)

    target_link_libraries(${PROJECT_NAME}_tests
      PRIVATE
      GTest::GTest
      GTest::Main
      ${PROJECT_NAME}_lib
    )
    target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    get_target_property(INCLUDE_DIRS ${PROJECT_NAME}_tests INCLUDE_DIRECTORIES)
    message(WARNING "Include directories for ${PROJECT_NAME}_tests: ${INCLUDE_DIRS}")

    enable_testing()
    add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
endif()
