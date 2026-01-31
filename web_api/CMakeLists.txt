add_library(web_api STATIC web_api.c
        include/web_api.h
        include/web_utils.h
        web_utils.c
        include/socket_platform.h
        include/web_server.h
        web_server.c
        include/web_routes.h
        web_routes.c
        include/web_app.h
        web_app.c)
target_include_directories(web_api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)