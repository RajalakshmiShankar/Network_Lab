#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(void) {
    // Initialize variables
    int socket_desc, client_sock;
    socklen_t client_size;
    struct sockaddr_in client_addr, server_addr;
    char client_message[2000], server_message[2000];
    char buffer[1024];
    int k = 5, m = 1, p;

    // Clean buffer that had been initialized
    memset(client_message, '\0', sizeof(client_message));
    memset(server_message, '\0', sizeof(server_message));

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        printf("Unable to create socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Binding not possible\n");
        return -1;
    }
    printf("Binding successful\n");

    // Listen for incoming connections
    if (listen(socket_desc, 1) < 0) {
        printf("Error while listening\n");
        return -1;
    }
    printf("Listening for incoming connections...\n");

    // Accept incoming connections
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    if (client_sock < 0) {
        printf("Can't accept connection\n");
        return -1;
    }
    printf("Client connected at IP: %s and port: %i\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Communication loop
    while (k != 0) {
        int y = recv(client_sock, buffer, sizeof(buffer), 0);
        if (y == -1) {
            printf("Error in receiving data\n");
            exit(1);
        }

        if (strncmp(buffer, "frame", 5) == 0) {
            printf("Received frame %d successfully\n", m);
        } else {
            printf("Frame %d not received successfully\n", m);
        }

        // Simulate acknowledgment loss and retransmission
        if (m % 2 == 0) {
            strcpy(buffer, "ack");
        } else {
            strcpy(buffer, "kca");
            printf("Ack lost\n");
            for (p = 1; p <= 3; p++) {
                printf("Waiting for %d seconds\n", p);
                sleep(1);  // Simulate delay with a 1-second pause between each print
            }
            printf("Retransmitting ack...\n");
            strcpy(buffer, "ack");
            sleep(3);  // Simulate delay before retransmission
        }

        printf("Sending ack %d\n", m);
        int z = send(client_sock, buffer, strlen(buffer), 0);  // Use the correct size of the message
        if (z == -1) {
            printf("Error in sending\n");
            exit(1);
        }

        k--;
        m++;
    }

    // Close the sockets
    close(client_sock);
    close(socket_desc);

    return 0;
}

