/*
 * main.c
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "web_app.h"
#include "web_server.h"


void root_handler(const eyxp_http_request_t *req, eyxp_http_response_t *res) {
    (void) req;
    response_json(res, 200, "OK", "{\"status\":\"ok\"}\n");
}

int main(void) {

    eyxp_app_t app = initApp();
    const socket_t webSocket = setup_socket();
    const socket_t webServer = setup_web_server(&webSocket, 8080);

    register_route(&app, "GET", "/", root_handler);
    register_route(&app, "POST", "/", root_handler);
    listen_web_server(&app, (socket_t*) &webServer);
}
