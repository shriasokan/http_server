#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define PORT 8081

void handle_connection(int client_fd);

int main(int argc, char **argv) {
    char *directory = ".";
    if (argc >= 3) {
        if (strcmp(argv[1], "--directory") == 0) {
            directory = argv[2];
        } 
    }
    printf("Setting up directory to %s\n", directory);

    if (chdir(directory) < 0) {
        printf("Failed to set current dir");
        return 1;
    }
    
    setbuf(stdout, NULL);

    printf("Starting program!\n");

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = {htonl(INADDR_ANY)},
    };

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Socket creation failed: %s...\n", strerror(errno));
        return 1;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEPORT failed %s \n", strerror(errno));
        return 1;
    }

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
        printf("Bind failed: %s \n", strerror(errno));
        return 1;
    }

    int connection_backlog = 5;

    if (listen(server_fd, connection_backlog) != 0) {
        printf("Listen failed: %s \n", strerror(errno));
        return 1;
    }

    while (1) {
        printf("Server started.\n");
        printf("\tWaiting for client to connect...\n");
        struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd == -1) {
            printf("Failed to connect: %s \n", strerror(errno));
            return 1;
        }

        printf("Client connected\n");
    }
}