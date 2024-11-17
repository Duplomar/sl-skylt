#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

const unsigned short port = ('S' << 8) + 'L';

void* connection_handler(void* vargp)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};  // Buffer to store the request

    printf("Creating socket file descriptior\n");
    // Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) 
    {
        perror("Socket failed");
        pthread_exit(NULL);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
    {
        perror("setsockopt failed");
        close(server_fd);
        pthread_exit(NULL);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    printf("Binding socket\n");
    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("Bind failed");
        close(server_fd);
        pthread_exit(NULL);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) 
    {
        perror("Listen failed");
        close(server_fd);
        pthread_exit(NULL);
    }

    printf("Server is listening on port %d\n", port);

    // Accept and handle incoming connections
    while (1) 
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) 
        {
            perror("Accept failed");
            close(server_fd);
            pthread_exit(NULL);
        }

        // Read the request from the client
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE - 1);  // Read up to BUFFER_SIZE - 1 bytes
        if (bytes_read < 0) 
        {
            perror("Read failed");
            close(new_socket);
            continue;
        }

        buffer[bytes_read] = '\0';  // Null-terminate the received data

        //////// insert update logic here
        printf("%s\n", buffer);

        // Close the socket for the client
        close(new_socket);
    }

    // Close the server socket
    close(server_fd);
    return 0;
} 
