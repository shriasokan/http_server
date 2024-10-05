# Basic HTTP Server (C and C++ Versions)
## Overview
This repository contains two implementations of a simple HTTP server -- one written in **C** and the other in 
**C++**. Both servers listen for incoming HTTP requests on a specified port and respond with basic HTTP responses,
such as returning an HTML page or echoing back parts of the request.
- The **C Version** focuses on a manual, low-level implementation using raw sockets, with detailed handling of
socket connections and HTTP response construction.
- The **C++ Version** adds more structure and object-oriented features, breaking the server into classes and modules
for better code organization and readability.

Both versions illustrate key concepts such as socket programming, handling multiple client connections (using `fork()`),
and sending HTTP responses. While the core functionality is similar, the code structure and approach differ to show how
C and C++ can handle similar tasks.

## Compilation and Running Instructions: C Version
1. **Navigate to the `C_version` directory** using the command: `cd C_version`
2. **Compile the C server** using the command: `gcc http_server.c -o http_server`
3. **Run the C server** using the command: `./http_server`
 - This will start the C-based HTTP server, which listens on port 8081 by default.
4. **Test the server** using the command: `curl http://localhost:8081` **OR** by visiting `http://localhost:8081/` in your browser

## Compilation and Running Instructions: C++ Version
1. **Navigate to the `C++_version` directory** using the command: `cd C++_version`
2. **Compile the C++ server** using the command: `g++ server_linux.cpp http_tcpServer_linux.cpp -o http_server`
3. **Run the C++ server** using the command: `./http_server`
 - This will start the C++-based HTTP server, which listen on port 8080 by default (or as configured in the code).
4. **Test the server** using the command: `curl http://localhost:8080` **OR** by visiting `http://localhost:8080`

## Notes:
- **Ports**: The C server listens on port **8081**, while the C++ server listens on port **8080**. You can adjust the ports by
changing the `#define PORT` or respective constants in the C or C++ code.
- **Permissions**: If you encounter permission errors or issues with port binding, you might need to run the servers using this
command: `sudo ./http_server`