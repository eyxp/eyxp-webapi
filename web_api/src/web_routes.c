/*
 * web_routes
 *
 * Author: eyxp
 * GitHub: https://github.com/eyxp
 * Created: 31.01.2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "web_app.h"
#include "web_routes.h"

#include <string.h>

int register_route(eyxp_app_t* app,
                   const char* method,
                   const char* path,
                   eyxp_route_handler_t route_handler)
{
    if (!app || !method || !path || !route_handler) return 1;
    if (app->count >= EYXP_ROUTE_MAX) return 2;

    app->routes[app->count++] = (eyxp_route_t){ method, path, route_handler };
    return 0;
}

void response_text(eyxp_http_response_t* response,
                   const int status,
                   const char* status_text,
                   const char* content_type,
                   const char* body)
{
    if (!response) return;

    memset(response, 0, sizeof(*response));

    response->status = status;
    response->status_text = status_text;
    response->content_type = content_type;
    response->body = body;
    response->body_length = body ? strlen(body) : 0;
}
