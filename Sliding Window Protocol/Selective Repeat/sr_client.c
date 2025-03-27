#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[80];
    int total_frames, window_size, frame_number = 0, count_ack = 0;
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error creating socket\n");
        return -1;
    }
    printf("Socket successfully created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        printf("Connection to the server failed\n");
        return -1;
    }
    printf("Connected to the server\n");

    printf("Enter the number of frames: ");
    scanf("%d", &total_frames);
    printf("Enter the window size: ");
    scanf("%d", &window_size);

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt(SO_RCVTIMEO) failed");
    }

    int w1 = 0, w2 = window_size - 1;
    while (frame_number < total_frames && frame_number <= w2) {
        snprintf(buffer, sizeof(buffer), "%d", frame_number);
        send(sockfd, buffer, sizeof(buffer), 0);
        printf("Frame %d sent\n", frame_number);
        frame_number++;
    }

    while (1) {
        if (count_ack == total_frames) {
            strcpy(buffer, "Exit");
            send(sockfd, buffer, sizeof(buffer), 0);
            break;
        }

        if (w2 - w1 != window_size - 1 && frame_number != total_frames) {
            snprintf(buffer, sizeof(buffer), "%d", frame_number);
            send(sockfd, buffer, sizeof(buffer), 0);
            printf("Frame %d sent\n", frame_number);
            w2++;
            frame_number++;
        }

        memset(buffer, 0, sizeof(buffer));
        int n = recv(sockfd, buffer, sizeof(buffer), 0);
        int ack = atoi(buffer);

        if (n > 0) {
            if (ack + 1 == total_frames) {
                printf("Acknowledgment received: %d\nExit\n", ack);
                count_ack++;
            } else if (ack == -1) {
                printf("Acknowledgment not received for %d\nResending frame\n", w1);
                snprintf(buffer, sizeof(buffer), "%d", w1);
                send(sockfd, buffer, sizeof(buffer), 0);
                printf("Frame %d sent\n", w1);
            } else {
                w1++;
                printf("Acknowledgment received: %d\n", ack);
                count_ack++;
            }
        }
    }

    close(sockfd);
    return 0;
}