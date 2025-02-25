#include <stdio.h>
#include <string.h>
#include <sys/socket.h> // Socket programming functions
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // UNIX standard functions (for close)

int main(void) {
    // Declaration of variables
    int socket_desc; // Socket descriptor for the client
    struct sockaddr_in server_addr; // Structure for server address
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

    // Connect to the server
    if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Unable to connect!\n");
        return -1; // Exit if connection fails
    }
    printf("Connected to server successfully!\n");

    // Get input from the user
    printf("Enter a message: ");
    fgets(client_message, sizeof(client_message), stdin); // Use fgets instead of gets for safety

    // Send the message to the server
    if (send(socket_desc, client_message, strlen(client_message), 0) < 0) { // Send the message
        printf("Unable to send the message!\n");
        return -1; // Exit if sending fails
    }
    printf("Message sent successfully\n");

    // Receive a response from the server
    if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) { // Receive data from the server
        printf("Error while receiving the message!\n");
        return -1; // Exit if receiving fails
    }
    printf("Server response: %s\n", server_message); // Print the server's response

    // Close the socket
    close(socket_desc); // Close the socket
    return 0; // Exit the program successfully
}