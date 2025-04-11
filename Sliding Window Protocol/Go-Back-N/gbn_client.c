#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int socket_desc;
    struct sockaddr_in servaddr;
    struct timeval timeout;
    char buffer[80];
    int f, w, ack = -1, i = 0, w1 = 0, w2, j, flag = 0;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created\n");

    // Configure server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(8080);

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    // Connect to server
    if (connect(socket_desc, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection with the server failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server\n");

    // Get frame and window size
    printf("Enter the number of frames: ");
    scanf("%d", &f);
    printf("Enter the window size: ");
    scanf("%d", &w);

    w2 = w - 1;

    if (setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0)
        perror("setsockopt(SO_RCVTIMEO) failed");

    // Send initial frames in the window
    for (i = 0; i < f && i <= w2; i++) {
        snprintf(buffer, sizeof(buffer), "%d", i);
        send(socket_desc, buffer, sizeof(buffer), 0);
        printf("Frame %d sent\n", i);
    }

    while (1) {
        flag = 0;
        memset(buffer, 0, sizeof(buffer));
        int n = recv(socket_desc, buffer, sizeof(buffer), 0);

        if (n <= 0) {
            printf("Acknowledgment timeout! Resending frames from %d\n", w1);
            for (j = w1; j < f && j < w1 + w; j++) {
                snprintf(buffer, sizeof(buffer), "%d", j);
                send(socket_desc, buffer, sizeof(buffer), 0);
                printf("Frame %d resent\n", j);
            }
            flag = 1;
            continue;
        }

        ack = atoi(buffer);

        if (ack + 1 == f) {
            printf("Final acknowledgment received: %d\nExit\n", ack);
            strcpy(buffer, "Exit");
            send(socket_desc, buffer, sizeof(buffer), 0);
            break;
        }

        if (ack >= w1) {
            w1 = ack + 1;
            printf("Acknowledgment received: %d\n", ack);
        }

        if (flag == 0 && i < f) {
            for (; i < f && i <= w1 + w - 1; i++) {
                snprintf(buffer, sizeof(buffer), "%d", i);
                send(socket_desc, buffer, sizeof(buffer), 0);
                printf("Frame %d sent\n", i);
            }
        }
    }

    close(socket_desc);
    return 0;
}