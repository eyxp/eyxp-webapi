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

```

