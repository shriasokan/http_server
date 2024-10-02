#include "http_tcpServer_linux.h"

#include <iostream>
#include <sstream>
#include <unistd.h>

namespace {
    const int BUFFER_SIZE = 30720;

    void log(const std::string &message) {
        std::cout << message << std::endl;
    }

    void exitWithError(const std::string &errorMessage) {
        log("ERROR: " + errorMessage);
        exit(1);
    }
}
namespace http {
    TcpServer::TcpServer(const std::string ip_address, int port) 
        : m_ip_address(ip_address), m_port(port), m_socket(-1), m_socketAddress_len(sizeof(m_socketAddress)) {
        m_socketAddress.sin_family = AF_INET;
        m_socketAddress.sin_port = htons(m_port);
        m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());
    }

    TcpServer::~TcpServer() {
        if (m_socket >= 0) {
            close(m_socket);
        }
    }

    int TcpServer::createSocket(){
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket < 0){
            std::cerr << "Error: Could not create socket." << std::endl;
            return -1;
        }

        int opt = 1;
        if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "Error: Could not set socket options (SO_REUSEADDR)." << std::endl;
            close(m_socket);
            return -1;
        }

        return 0;
    }

    int TcpServer::bindSocket() {
        if (bind(m_socket, (struct sockaddr*)&m_socketAddress, m_socketAddress_len) < 0) {
            std::cerr << "Error: Could not bind to " << m_ip_address << ":" << m_port << std::endl;
            close(m_socket);
            return -1;
        }
        
        return 0;
    }

    int TcpServer::start() {
        if (createSocket() < 0) {
            return -1;
        }

        if (bindSocket() < 0) {
            return -1;
        }

        std::cout << "Server started successfully on " << m_ip_address << ":" << m_port << std::endl;
        return 0;
    }

    void TcpServer::listenForConnections() {
        if (listen(m_socket, 20) < 0) {
            std::cerr << "Error: Failed to listen on socket." << std::endl;
            return;
        }

        std::cout << "Listening for incoming connections..." << std::endl;

        while (true) {
            int client_socket = accept(m_socket, nullptr, nullptr);
            if (client_socket < 0) {
                std::cerr << "Error: Failed to accept connection." << std::endl;
                continue;
            }
            handleClient(client_socket);
            close(client_socket);
        }
    }

    void TcpServer::handleClient(int client_socket) {
        char buffer[1024] = {0};
        read(client_socket, buffer, sizeof(buffer));
        std::cout << "Received request: " << buffer << std::endl;
        std::string response = buildResponse();
        send(client_socket, response.c_str(), response.size(), 0);
    }

    std::string TcpServer::buildResponse() {
        std::string html = "<html><body><h1>Hello from Server!</h1></body></html>";
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << html.size() << "\r\n";
        response << "\r\n";
        response << html;
        return response.str();
    }
}
