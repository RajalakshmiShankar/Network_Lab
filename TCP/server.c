#include <stdio.h>
#include <string.h>
#include <sys/socket.h> // Socket programming functions
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // UNIX standard functions (for close)

int main(void) {
    // Declaration of variables
    int socket_desc, client_sock, client_size; // Socket descriptors
    struct sockaddr_in server_addr, client_addr; // Structures for server and client addresses
    char server_message[2000], client_message[2000]; // Buffers for messages

    // Clean buffer
    memset(server_message, '\0', sizeof(server_message)); // Initialize server message buffer to zero
    memset(client_message, '\0', sizeof(client_message)); // Initialize client message buffer to zero

    // Create the socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket
    if (socket_desc < 0) { // Check if socket creation failed
        printf("Socket not created\n");
        return -1; // Exit if socket creation fails
    }
    printf("Socket created successfully\n");

    // Server and port configuration
    server_addr.sin_family = AF_INET; // Use IPv4
    server_addr.sin_port = htons(2000); // Set port number (2000) and convert to network byte order
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set IP address to localhost

    // Binding the socket to the address and port
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) { // Corrected the cast and condition
        printf("Binding not successful\n"); // Print error if binding fails
        return -1; // Exit if binding fails
    }
    printf("Binding is successful\n"); // Print success message for binding

    // Start listening for incoming connections
    if (listen(socket_desc, 3) < 0) { // Listen for up to 3 connections in the queue
        printf("Listening failed\n"); // Print error if listening fails
        return -1; // Exit if listening fails
    }
    printf("Listening for incoming connections...\n"); // Print message indicating server is listening

    // Accept a connection from a client
    client_size = sizeof(client_addr); // Get the size of the client address structure
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, (socklen_t*)&client_size); // Accept the connection
    if (client_sock < 0) { // Check if acceptance failed
        printf("Connection acceptance failed\n"); // Print error message
        return -1; // Exit if acceptance fails
    }
    printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); // Print client IP and port

    // Receive a message from the client
    int read_size = recv(client_sock, client_message, sizeof(client_message), 0); // Receive data from the client
    if (read_size > 0) { // Check if data was received
        printf("Client message: %s\n", client_message); // Print the received message
    } else {
        printf("Failed to receive message\n"); // Print error if no message was received
    }

    // Prepare the response message
    strcpy(server_message, "This is the server message!"); // Copy response message into server_message

    // Send the response back to the client
    if (send(client_sock, server_message, strlen(server_message), 0)<0){
        printf("Response Cant be send");
    } // Send the response to the client
    printf("Response sent to client\n"); // Print confirmation of response sent

    // Close the client socket and server socket
    close(client_sock); // Close the client socket
    close(socket_desc); // Close the server socket
    return 0; // Exit the program successfully
}