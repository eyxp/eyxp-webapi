# eyxp-webapi

Eine kleine, portable Web-API in C (HTTP/1.1) mit einfachem Router (Method + Path), Text- und JSON-Responses und möglichst wenig Abhängigkeiten.

## Features
- Einfaches Routing: Method + Path
- Request-Parsing (Method/Path, optional Body/Content-Length je nach Implementierung)
- Response-Helpers für Text und JSON
- Kompiliert mit CMake (z.B. CLion/MinGW)

## Projektstruktur
- `web_api/include/web_api.h` – Request/Response Typen + HTTP-Funktionen + `app_handle`
- `web_api/include/web_routes.h` – `eyxp_route_t`, `register_route`, Response-Helper
- `web_api/include/web_app.h` – `eyxp_app_t` (Route-Liste) + `initApp`
- `web_api/include/web_server.h` – Socket-Setup + `listen_web_server`

## Build
```bash
cmake -S . -B build
cmake --build build -j
```
## Usage/Examples

```c
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
    eyxp_app_t app = eyxp_app_init(); // creates an new app instance
    const socket_t sock = setup_socket(); // setups client socket
    const socket_t server = setup_web_server(&sock, 8080); // add socket adress and configuration
    
    register_route(&app, "GET", "/", root_handler2); // registern an rout for /
    register_route(&app, "GET", "/:id/:name", root_handler); // registern and route for /id/name
    listen_web_server(&app, (socket_t*)&server); // listen for incoming request
    return 0; 
}


```

