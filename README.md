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
