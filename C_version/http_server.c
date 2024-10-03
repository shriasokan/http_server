#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 5

void handle_connection(int client_fd);
int create_server_socket(struct sockaddr_in *serv_addr);
int bind_server_socket(int server_fd, struct sockaddr_in *serv_addr);
int start_listening(int server_fd);

int main(int argc, char **argv) {
    char *directory = ".";
    if (argc >= 3 && strcmp(argv[1], "--directory") == 0) {
        directory = argv[2];
    }
    printf("Setting up directory to %s\n", directory);

    if (chdir(directory) < 0) {
        perror("Failed to set current directory");
        return 1;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = {htonl(INADDR_ANY)},
    };

    int server_fd = create_server_socket(&serv_addr);
    if (server_fd < 0) return 1;

    if (bind_server_socket(server_fd, &serv_addr) < 0) return 1;

    if (start_listening(server_fd) < 0) return 1;

    while (1) {
        printf("Server started. Waiting for client to connect... \n");
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd == -1) {
            perror("Failed to accept connection");
            continue;
        }

        printf("Client connected\n");

        if (!fork()) {
            close(server_fd);
            handle_connection(client_fd);
            close(client_fd);
            exit(0);
        }
        close(client_fd);
    }

    return 0;
}

int create_server_socket(struct sockaddr_in *serv_addr) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    int reuse =1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) <0) {
        perror("Failed to set socket option");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

int bind_server_socket(int server_fd, struct sockaddr_in *serv_addr) {
    if (bind(server_fd, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) != 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }
    return 0;
}

int start_listening(int server_fd) {
    if (listen(server_fd, BACKLOG) != 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }
    return 0;
}

void handle_connection(int client_fd) {
    char readBuffer[1024];
    int bytesReceived = recv(client_fd, readBuffer, sizeof(readBuffer), 0);

    if (bytesReceived == -1) {
        perror("Receiving failed");
        exit(1);
    }

    char *method = strtok(readBuffer, " ");
    char *reqPath = strtok(NULL, " ");
    printf("Request Path: %s\n", reqPath);

    int bytesSent;
    if (strcmp(reqPath, "/") == 0) {
        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                         "<html><body><h1>C HTTP Server</h1></body></html>";
        bytesSent = send(client_fd, response, strlen(response), 0);
    }
    else if (strncmp(reqPath, "/echo/", 6) == 0) {
        char *echoMessage = reqPath + 6;
        char response[512];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %ld\r\n\r\n%s",
                strlen(echoMessage), echoMessage);
        bytesSent = send(client_fd, response, strlen(response), 0);
    }
    else {
        char *notFound = "HTTP/1.1 404 Not Found\r\n\r\n";
        bytesSent = send(client_fd, notFound, strlen(notFound), 0);
    }

    if (bytesSent < 0) {
        perror("Send failed");
        exit(1);
    }
}