#include "web_utils.h"
#include "web_routes.h"
#include <string.h>
#include <stdio.h>

#include "web_app.h"

#ifndef EYXP_MIN
#define EYXP_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

int http_parse_request(const char *request_buffer, eyxp_http_request_t *output_request) {
    if (!request_buffer || !output_request) {
        return 1;
    }

    memset(output_request, 0, sizeof(*output_request));

    if (sscanf(request_buffer, "%7s %255s", output_request->method, output_request->path) != 2) {
        return 1;
    }

    const char *header_end = find_header_end(request_buffer);
    if (!header_end) {
        return 2;
    }

    const char *header_start = strstr(request_buffer, "\r\n");
    if (!header_start) {
        return 1;
    }
    header_start += 2;

    output_request->content_length = parse_content_length(header_start);

    const char *body_start = header_end + 4;
    size_t available_body_bytes = strlen(body_start);

    if (output_request->content_length > available_body_bytes) {
        return 2;
    }

    output_request->body = body_start;
    output_request->body_length = output_request->content_length;
    return 0;
}

const char *eyxp_next_path_segment_end(const char *segment_start) {
    const char *slash_position = strchr(segment_start, '/');
    if (slash_position) {
        return slash_position;
    }
    return segment_start + strlen(segment_start);
}

void eyxp_copy_token_to_buffer(char *destination_buffer,
                               size_t destination_capacity,
                               const char *token_start,
                               size_t token_length) {
    if (!destination_buffer || destination_capacity == 0) {
        return;
    }

    size_t bytes_to_copy = EYXP_MIN(token_length, destination_capacity - 1);
    if (bytes_to_copy > 0) {
        memcpy(destination_buffer, token_start, bytes_to_copy);
    }
    destination_buffer[bytes_to_copy] = '\0';
}

int match_route(const eyxp_route_t *route,
                const char *request_path,
                eyxp_http_request_t *output_request) {
    if (!route || !request_path || !output_request) {
        return 0;
    }

    if (strcmp(route->method, output_request->method) != 0) {
        return 0;
    }

    const char *route_path_cursor = route->path;
    const char *request_path_cursor = request_path;

    output_request->param_count = 0;

    for (;;) {
        while (*route_path_cursor == '/') {
            route_path_cursor++;
        }
        while (*request_path_cursor == '/') {
            request_path_cursor++;
        }

        if (*route_path_cursor == '\0' && *request_path_cursor == '\0') {
            return 1;
        }

        if (*route_path_cursor == '\0' || *request_path_cursor == '\0') {
            return 0;
        }

        const char *route_segment_end = eyxp_next_path_segment_end(route_path_cursor);
        const char *request_segment_end = eyxp_next_path_segment_end(request_path_cursor);

        if (*route_path_cursor == ':') {
            if (output_request->param_count >= EYXP_MAX_ROUTE_PARAMS) {
                return 0;
            }

            eyxp_path_param_t *output_parameter = &output_request->params[output_request->param_count++];

            const char *parameter_name_start = route_path_cursor + 1;
            size_t parameter_name_length = (size_t) (route_segment_end - parameter_name_start);

            const char *parameter_value_start = request_path_cursor;
            size_t parameter_value_length = (size_t) (request_segment_end - parameter_value_start);

            eyxp_copy_token_to_buffer(output_parameter->name,
                                      EYXP_MAX_PARAM_NAME,
                                      parameter_name_start,
                                      parameter_name_length);

            eyxp_copy_token_to_buffer(output_parameter->value,
                                      EYXP_MAX_PARAM_VALUE,
                                      parameter_value_start,
                                      parameter_value_length);
        } else {
            size_t route_segment_length = (size_t) (route_segment_end - route_path_cursor);
            size_t request_segment_length = (size_t) (request_segment_end - request_path_cursor);

            if (route_segment_length != request_segment_length) {
                return 0;
            }

            if (memcmp(route_path_cursor, request_path_cursor, route_segment_length) != 0) {
                return 0;
            }
        }

        route_path_cursor = (*route_segment_end == '/') ? (route_segment_end + 1) : route_segment_end;
        request_path_cursor = (*request_segment_end == '/') ? (request_segment_end + 1) : request_segment_end;
    }
}

void app_handle(const eyxp_app_t *app,
                const eyxp_http_request_t *request,
                eyxp_http_response_t *response) {
    if (!app || !request || !response) {
        return;
    }

    for (size_t route_index = 0; route_index < app->count; ++route_index) {
        eyxp_http_request_t request_copy = *request;

        if (match_route(&app->routes[route_index], request->path, &request_copy)) {
            app->routes[route_index].handler(&request_copy, response);
            return;
        }
    }

    response_text(response, 404, "Not Found", "Not Found\n");
}

int http_send_response(socket_t client_socket, const eyxp_http_response_t *response) {
    if (!response || !response->status_text || !response->content_type) {
        return 1;
    }
    if (response->body_length > 0 && response->body == NULL) {
        return 1;
    }

    char header_buffer[512];

    const int header_bytes_written = snprintf(header_buffer, sizeof(header_buffer),
                                              "HTTP/1.1 %d %s\r\n"
                                              "Content-Type: %s\r\n"
                                              "Content-Length: %zu\r\n"
                                              "Connection: close\r\n"
                                              "\r\n",
                                              response->status,
                                              response->status_text,
                                              response->content_type,
                                              response->body_length);

    if (header_bytes_written <= 0) {
        return 1;
    }
    if ((size_t) header_bytes_written >= sizeof(header_buffer)) {
        return 1;
    }

    if (send_all(client_socket, header_buffer, (size_t) header_bytes_written) != 0) {
        return 1;
    }

    if (response->body_length > 0) {
        if (send_all(client_socket, response->body, response->body_length) != 0) {
            return 1;
        }
    }

    return 0;
}
