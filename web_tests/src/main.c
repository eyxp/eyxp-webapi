/*
 * main.c
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

#include "web_app.h"
#include "web_server.h"


void root_handler(const eyxp_http_request_t *request, eyxp_http_response_t *response) {
    printf("root handler called name: %s\n", eyxp_param_get(request, "name"));
    response_json(response, 200, "OK", "{\"status\":\"ok\"}\n");
}

void root_handler2(const eyxp_http_request_t *request, eyxp_http_response_t *response) {
    response_json(response, 200, "OK", "{\"status\":\"ok\"}\n");
}

int main(void) {
    eyxp_app_t app = eyxp_app_init();
    const socket_t sock = setup_socket();
    const socket_t server = setup_web_server(&sock, 8080);


    register_route(&app, "GET", "/", root_handler2);
    register_route(&app, "GET", "/:id/:name", root_handler);
    listen_web_server(&app, (socket_t*)&server);
    return 0;
}
