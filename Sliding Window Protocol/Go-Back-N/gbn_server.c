#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
    int socket_desc, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_size;
    char buffer[1024];
    int expected_frame = 0, frame_number = 1, ack;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        perror("Error while creating socket");
        return -1;
    }
    printf("Socket created successfully\n");

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to the port
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Couldn't bind to port");
        return -1;
    }
    printf("Done with binding\n");

    // Start listening for incoming connections
    if (listen(socket_desc, 1) < 0) {
        perror("Error while listening");
        return -1;
    }
    printf("\nListening for incoming connections...\n");

    // Accept client connection
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    if (client_sock < 0) {
        perror("Can't accept connection");
        return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Frame reception
    while (1) {
        //memset(buffer, 0, sizeof(buffer));
        
        if (recv(client_sock, buffer, sizeof(buffer), 0) <= 0) {
            printf("Client disconnected or error in receiving.\n");
            break;
        }

        if (strcmp(buffer, "Exit") == 0) {
            printf("Exit received. Closing connection.\n");
            break;
        }

        int frame_num = atoi(buffer);

        if (frame_num == expected_frame) {
            printf("Frame %d received correctly.\n", frame_num);
            ack = frame_num;
            expected_frame++;  // Move to next expected frame
        } else {
            printf("Frame %d out of order. Resending last ACK %d.\n", frame_num, ack);
        }
        

        // Send acknowledgment
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%d", ack);
        send(client_sock, buffer, sizeof(buffer), 0);
    }

    close(client_sock);
    close(socket_desc);
    return 0;
}