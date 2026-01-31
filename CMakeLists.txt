cmake_minimum_required(VERSION 3.20)
project(eyxp_webapi C)

set(CMAKE_C_STANDARD 23)

add_subdirectory(web_api)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/web_tests/CMakeLists.txt")
    add_subdirectory(web_tests)
endif ()