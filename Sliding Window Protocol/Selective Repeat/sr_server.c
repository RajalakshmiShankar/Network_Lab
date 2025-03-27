#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char buffer[80];
    int frame_number, ack, total_frames = 5, received_frames[5], expected_frame = 0;

    for (int i = 0; i < total_frames; i++) {
        received_frames[i] = 0;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error creating socket\n");
        return -1;
    }
    printf("Socket successfully created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Socket bind failed\n");
        return -1;
    }
    printf("Socket successfully bound\n");

    if (listen(sockfd, 1) < 0) {
        printf("Listen failed\n");
        return -1;
    }
    printf("Server listening...\n");

    client_size = sizeof(client_addr);
    client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &client_size);
    if (client_sock < 0) {
        printf("Server accept failed\n");
        return -1;
    }
    printf("Client connected at IP: %s and port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (1) {
        sleep(1);
        memset(buffer, 0, sizeof(buffer));

        if (recv(client_sock, buffer, sizeof(buffer), 0) <= 0) {
            printf("Receive failed\n");
            break;
        }

        if (strcmp(buffer, "Exit") == 0) {
            printf("Exit command received. Closing connection.\n");
            break;
        }

        frame_number = atoi(buffer);
        int c = rand() % 3;  // Simulate random errors

        switch (c) {
            case 0:
                printf("Frame %d not received\n", frame_number);
                ack = -1;
                printf("Negative Acknowledgment sent: %d\n", frame_number);
                snprintf(buffer, sizeof(buffer), "%d", ack);
                send(client_sock, buffer, sizeof(buffer), 0);
                break;
            case 1:
                ack = frame_number;
                sleep(2);
                printf("Frame %d received\nAcknowledgment sent: %d\n", frame_number, ack);
                snprintf(buffer, sizeof(buffer), "%d", ack);
                send(client_sock, buffer, sizeof(buffer), 0);
                received_frames[frame_number] = 1;
                break;
            case 2:
                ack = frame_number;
                printf("Frame %d received\nAcknowledgment sent: %d\n", frame_number, ack);
                snprintf(buffer, sizeof(buffer), "%d", ack);
                send(client_sock, buffer, sizeof(buffer), 0);
                received_frames[frame_number] = 1;
                break;
        }

        while (received_frames[expected_frame] == 1) {
            expected_frame++;
        }
    }

    close(client_sock);
    close(sockfd);
    return 0;
}