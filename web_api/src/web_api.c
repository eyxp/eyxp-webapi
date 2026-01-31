#include "web_api.h"

#include "socket_platform.h"
#include "web_app.h"
#include "web_utils.h"
#include "web_routes.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int http_parse_request(const char* buffer, eyxp_http_request_t* output) {
    memset(output, 0, sizeof(*output));

    if (sscanf(buffer, "%7s %255s", output->method, output->path) != 2) {
        return 1;
    }

    char* header_end = find_header_end(buffer);
    if (!header_end) return 2;

    char* header_start = strstr(buffer, "\r\n");
    if (!header_start) return 1;
    header_start += 2;

    output->content_length = parse_content_length(header_start);

    const char* body = header_end + 4;
    size_t available  = strlen(body);

    if (output->content_length > available) return 2;

    output->body = body;
    output->body_length = output->content_length;
    return 0;
}

void app_handle(const eyxp_app_t* app, const eyxp_http_request_t* request, eyxp_http_response_t* response) {
    if (!app || !request || !response) return;

    for (size_t i = 0; i < app->count; i++) {
        const eyxp_route_t* route = &app->routes[i];
        if (strcmp(route->method, request->method) == 0 && strcmp(route->path, request->path) == 0) {
            route->route_handler(request, response);
            return;
        }
    }
    response_text(response, 404, "Not Found",nullptr);
}

int http_send_response(const socket_t socket, const eyxp_http_response_t* response) {
    if (!response || !response->status_text || !response->content_type) return 1;
    if (response->body_length > 0 && response->body == NULL) return 1;

    char hdr[512];
    const int hn = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        response->status,
        response->status_text,
        response->content_type,
        response->body_length);

    if (hn <= 0 || (size_t)hn >= sizeof(hdr)) return 1;

    if (send_all(socket, hdr, (size_t)hn) != 0) return 1;
    if (response->body_length > 0) {
        if (send_all(socket, response->body, response->body_length) != 0) return 1;
    }
    return 0;
}
