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
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd == -1) {
            printf("Failed to connect: %s \n", strerror(errno));
            return 1;
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

void handle_connection(int client_fd) {
    printf("\n");

    char readBuffer[1024];
    int bytesReceived = recv(client_fd, readBuffer, sizeof(readBuffer), 0);

    if (bytesReceived == -1) {
        printf("Receiving failed: %s \n", strerror(errno));
        exit(1);
    }

    char *method = strdup(readBuffer);
    char *content = strdup(readBuffer);
    printf("Content: %s\n", method);
    char *reqPath = strtok(readBuffer, " ");
    reqPath = strtok(NULL, " ");

    int bytesSent;

    if (strcmp(reqPath, "/") == 0) {
        char *res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                    "<html><body><h1>C HTTP Server</h1></body></html>";
        printf("Sending reponse: %s/n", res);
        bytesSent = send(client_fd, res, strlen(res), 0);
    }
    else if (strncmp(reqPath, "/echo/", 6) == 0) {
        reqPath = strtok(reqPath, "/");
        reqPath = strtok(NULL, "");
        int contentLength = strlen(reqPath);

        char response[512];
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n%s", contentLength, reqPath);
        printf("Sending response: %s\n", response);
        bytesSent = send(client_fd, response, strlen(response), 0);
    }
    else if (strcmp(reqPath, "/user-agent") == 0) {
        reqPath = strtok(NULL, "\r\n");
        reqPath = strtok(NULL, "\r\n");
        reqPath = strtok(NULL, "\r\n");

        char *body = strtok(reqPath, " ");
        body = strtok(NULL, " ");
        int contentLength = strlen(body);

        char response[512];
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n%s", contentLength, body);
        printf("Sending response: %s\n", response);
        bytesSent = send(client_fd, response, strlen(response), 0);
    }
    else if (strncmp(reqPath, "/files/", 7) == 0 && strcmp(method, "GET") == 0) {
        char *filename = strtok(NULL, "");
        filename = strtok(NULL, "");

        FILE *fp = fopen(filename, "rb");
        if (!fp) {
            printf("File not found");
            char *res = "HTTP/1.1 404 Not Found\r\n\r\n";
            bytesSent = send(client_fd, res, strlen(res), 0);
        }
        else {
            printf("Opening file %s\n", filename);
        }

        if (fseek(fp, 0, SEEK_END) < 0) {
            printf("Error reading the document\n");
        }

        size_t data_size = ftell(fp);
        rewind(fp);
        void *data = malloc(data_size);

        if (fread(data, 1, data_size, fp) != data_size) {
            printf("Error reading the document\n");
        }

        fclose(fp);
        char response[1024];
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %ld\r\n\r\n%s", data_size, (char *)data);
        printf("Sending response: %s\n", response);
        bytesSent = send(client_fd, response, strlen(response), 0);
    }
    else if (strncmp(reqPath, "/files/", 7) == 0 && strcmp(method, "POST") == 0) {
        method = strtok(NULL, "\r\n");
        method = strtok(NULL, "\r\n");
        method = strtok(NULL, "\r\n");
        method = strtok(NULL, "\r\n");
        method = strtok(NULL, "\r\n");
        
        char *contentLengthStr = strtok(method, " ");
        contentLengthStr = strtok(NULL, " ");
        int contentLength = atoi(contentLengthStr);
        char *filename = strtok(reqPath, "/");
        filename = strtok(NULL, "");

        content = strtok(content, "\r\n");
        content = strtok(NULL, "\r\n");
        content = strtok(NULL, "\r\n");
        content = strtok(NULL, "\r\n");
        content = strtok(NULL, "\r\n");
        content = strtok(NULL, "\r\n");
        content = strtok(NULL, "\r\n");

        printf("\n---\nCreate a file %s with content length: %d\n\n %s\n---\n", filename, contentLength, content);
        FILE *fp = fopen(filename, "wb");
        if (!fp) {
            printf("File could not be opened");
            char *res = "HTTP/1.1 404 Not Found\r\n\r\n";
            bytesSent = send(client_fd, res, strlen(res), 0);
        }
        else {
            printf("Opening file %s\n", filename);
        }

        if (fwrite(content, 1, contentLength, fp) != contentLength) {
            printf("Error writing the data");
        }

        fclose(fp);
        char response[1024];
        sprintf(response, "HTTP/1.1 201 Created\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n%s", contentLength, content);
        printf("Sending response: %s\n", response);
        bytesSent = send(client_fd, response, strlen(response), 0);
    }
    else {
        char *res = "HTTP/1.1 404 Not Found\r\n\r\n";
        bytesSent = send(client_fd, res, strlen(res), 0);
    }
    if (bytesSent < 0) {
        printf("Send failed\n");
        exit(1);
    }
    else {
        return;
    }
}