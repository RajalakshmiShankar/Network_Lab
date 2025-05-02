#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {

    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    int frame_number = 1, frames_to_receive = 5;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        printf("ERROR while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Couldn't bind to port\n");
        return -1;
    }
    printf("Done with binding\n");

    if (listen(socket_desc, 1) < 0) {
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections......\n");

    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    if (client_sock < 0) {
        printf("Can't accept\n");
        return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (frames_to_receive > 0) {
        if (recv(client_sock, buffer, sizeof(buffer), 0) < 0) {
            printf("Error in receiving\n");
            exit(1);
        }

        if (frame_number % 2 != 0) {
            printf("Acknowledgment lost for frame %d\n", frame_number);
            sleep(3);
            printf("Retransmitting acknowledgment......\n");
        }

        strcpy(buffer, "ack");
        printf("Sending acknowledgment for frame %d\n", frame_number);

        if (send(client_sock, buffer, 19, 0) < 0) {
            printf("Error in sending\n");
            exit(1);
        }

        frames_to_receive--;
        frame_number++;
    }

    close(client_sock);
    close(socket_desc);
    return 0;
}

