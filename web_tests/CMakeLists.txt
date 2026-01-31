add_executable(web_tests main.c
        main.c)
target_link_libraries(web_tests PRIVATE web_api)

if (WIN32)
    target_link_libraries(web_tests PRIVATE ws2_32)
endif()