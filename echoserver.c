/*Socket programming is a way of connecting two nodes on a network to communicate with each other. 
One socket(node) listens on a particular port at an IP, while the other socket reaches out to the 
other to form a connection.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DOMAIN AF_INET // AF_INET used for communicating over a network, communication domain
#define TYPE SOCK_STREAM // communication type
#define PROTOCOL 0
#define BUFFER_SIZE 1024

// server address
struct sockaddr_in server_address = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = INADDR_ANY,
    .sin_port = 0  
};

// handles client response
void echo_loop(int client_socket_desc){
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = recv(client_socket_desc, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Received: %s", buffer);

        ssize_t bytes_sent = send(client_socket_desc, buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            perror("Send failed");
            break;
        }
    }

    if (bytes_read == -1) {
            perror("Receive failed");
        } else if (bytes_read == 0) {
            printf("Client disconnected\n");
        }
        close(client_socket_desc);
}

int main(int argc, char *argv[]){
    int port;
    int socket_desc; 

    if (argc != 3 || strcmp(argv[1], "-p") != 0) {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    port = atoi(argv[2]);

    if ((socket_desc = socket(DOMAIN, TYPE, PROTOCOL)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_port = htons(port); // set the port

    // bind server to specified port
    bind(socket_desc, (struct sockaddr*)&server_address, sizeof(server_address));
    // listen for connections
    listen(socket_desc, 1) == -1;

    while (1) {
        // client server address 
        struct sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);

        // accept a new connection
        int client_socket_desc = accept(socket_desc, (struct sockaddr*)&client_address, &client_addr_len);
        if (client_socket_desc == -1) {
            perror("Accept failed");
            continue; 
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        echo_loop(client_socket_desc);
    }

    close(socket_desc);

    return 0;
}