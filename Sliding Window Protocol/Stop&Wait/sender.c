#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(void) {
    // Initialize variables
    int socket_desc;
    struct sockaddr_in server_addr;
    char buffer[1024];
    int k = 5, m = 1, p;

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
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Connect to localhost

    // Connect to server
    if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected to the server successfully\n");

    // Communication loop
    while (k != 0) {
        if (m <= 5) {
            printf("Sending frame %d\n", m);
        }

        // Simulate packet loss and retransmission
        if (m % 2 == 0) {
            strcpy(buffer, "frame");
        } else {
            strcpy(buffer, "error");
            printf("Packet loss\n");
            for (p = 1; p <= 3; p++) {
                printf("Waiting for %d seconds\n", p);
                sleep(1);
            }
            printf("Retransmitting...\n");
            strcpy(buffer, "frame");
            sleep(3);
        }

        // Send frame to server
        int y = send(socket_desc, buffer, strlen(buffer), 0);
        if (y == -1) {
            printf("Error in sending data\n");
            exit(1);
        } else {
            printf("Sent frame %d\n", m);
        }

        // Receive acknowledgment from server
        int z = recv(socket_desc, buffer, sizeof(buffer), 0);
        if (z == -1) {
            printf("Error in receiving acknowledgment\n");
        }

        if (strncmp(buffer, "ack", 3) == 0) {
            printf("Received Ack for frame %d\n", m);
        }

        k--;
        m++;
    }

    // Close the socket
    close(socket_desc);

    return 0;
}

