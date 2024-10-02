#include "http_tcpServer_linux.h"
#include <iostream>

int main(){
    using namespace http;
    TcpServer server = TcpServer("0.0.0.0", 8080);

    if (server.start() == 0) {
        server.listenForConnections();
    } else {
        std::cerr << "Failed to start the server." << std::endl;
    }

    return 0;
}