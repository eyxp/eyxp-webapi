add_library(web_api STATIC library.c
        include/library.h)
target_include_directories(web_api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)