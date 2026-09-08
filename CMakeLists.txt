cmake_minimum_required(VERSION 3.16)

project(SummerMUD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SUMMER_MUD_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp")

add_executable(Summer-MUD ${SUMMER_MUD_SOURCES})
set_target_properties(Summer-MUD PROPERTIES OUTPUT_NAME "铃响之前")
if(MSVC)
    target_compile_options(Summer-MUD PRIVATE /utf-8)
endif()

include(CTest)
if(BUILD_TESTING)
    set(GAME_CORE_SOURCES ${SUMMER_MUD_SOURCES})
    list(FILTER GAME_CORE_SOURCES EXCLUDE REGEX "/main\\.cpp$")
    foreach(TEST_NAME StoryIntegration CoreRegression)
        add_executable(${TEST_NAME} tests/${TEST_NAME}.cpp ${GAME_CORE_SOURCES})
        if(MSVC)
            target_compile_options(${TEST_NAME} PRIVATE /utf-8 /UNDEBUG)
        else()
            target_compile_options(${TEST_NAME} PRIVATE -UNDEBUG)
        endif()
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test-work/${TEST_NAME}")
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        set_tests_properties(${TEST_NAME} PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test-work/${TEST_NAME}")
    endforeach()
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME RuntimeRegression COMMAND ${Python3_EXECUTABLE}
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/runtime_regression.py" $<TARGET_FILE:Summer-MUD>)
    endif()
endif()
