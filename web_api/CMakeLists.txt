add_library(web_api STATIC src/web_api.c
        include/web_api.h
        include/web_utils.h
        src/web_utils.c
        include/socket_platform.h
        include/web_server.h
        src/web_server.c
        include/web_routes.h
        src/web_routes.c
        include/web_app.h
        src/web_app.c
        include/web_config.h)
target_include_directories(web_api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)