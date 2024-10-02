#ifndef INCLUDED_HTTP_TCPSERVER_LINUX
#define INCLUDED_HTTP_TCPSERVER_LINUX

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>

namespace http
{
    class TcpServer
    {
        public:
            TcpServer(const std::string ip_address, int port);
            ~TcpServer();
            int start();
            void listenForConnections();

        private:
            std::string m_ip_address;
            int m_port;
            int m_socket;
            struct sockaddr_in m_socketAddress;
            socklen_t m_socketAddress_len;

            int createSocket();
            int bindSocket();
            void handleClient(int client_socket);
            std::string buildResponse();
    };
}
#endif