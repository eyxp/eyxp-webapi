add_executable(web_tests main.c
        main.c)
target_link_libraries(web_tests PRIVATE web_api)