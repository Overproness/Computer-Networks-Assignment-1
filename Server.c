#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT_NUMBER 8877 // Port number on which the server listens for connections
#define BUFFER_SIZE 1024 // Maximum size of the buffer for reading data

int main()
{
    // Step 1: Define variables for socket setup and initialize them
    struct sockaddr_in server_addr, client_addr;     // Structures to hold the server and client address info
    socklen_t client_addr_len = sizeof(client_addr); // Length of the client address structure
    int server_socket, client_socket;                // Sockets for listening and accepting client connections

    // Zero out the server address structure and configure it
    memset(&server_addr, 0, sizeof(server_addr));    // Clear the server address memory
    server_addr.sin_family = AF_INET;                // Use IPv4 addressing
    server_addr.sin_port = htons(PORT_NUMBER);       // Set the port number (convert to network byte order)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Allow connections from any IP address

    // Step 2: Create a listening socket
    if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: Unable to create listening socket."); // Print error if socket creation fails
        return 1;
    }

    // Step 3: Set socket options to allow address reuse (prevents binding errors on restart)
    int socket_option = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_option, sizeof(socket_option)) < 0)
    {
        perror("Error: Unable to set socket options."); // Print error if setting socket options fails
        return 1;
    }

    // Step 4: Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error: Socket binding failed."); // Print error if binding fails
        return 1;
    }

    // Step 5: Start listening for incoming client connections
    if (listen(server_socket, 16) < 0)
    {
        perror("Error: Unable to listen for incoming connections."); // Print error if listening fails
        return 1;
    }

    // Step 6: Accept an incoming connection from a client
    printf("Server is waiting for a client to connect...\n");
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
    {
        perror("Error: Failed to accept client connection."); // Print error if accepting connection fails
        return 1;
    }

    // Notify about successful client connection and display client's IP address
    printf("Client connected successfully with IP address: %s\n", inet_ntoa(client_addr.sin_addr));

    // Step 7: Open the video file to be sent to the client
    int video_file_fd = open("testVideo.mp4", O_RDONLY); // Open the video file in read-only mode
    if (video_file_fd < 0)
    {
        perror("Error: Unable to open video file for reading."); // Print error if file cannot be opened
        close(client_socket);                                    // Close the client socket on error
        return 1;
    }

    // Step 8: Prepare to read data from the file and send it to the client
    char data_buffer[BUFFER_SIZE]; // Buffer to hold file data
    ssize_t read_bytes;            // Variable to store the number of bytes read from the file

    // Step 9: Read the file and send the data in chunks to the client
    while ((read_bytes = read(video_file_fd, data_buffer, sizeof(data_buffer))) > 0)
    {
        send(client_socket, data_buffer, read_bytes, 0); // Send data chunk to the client
    }

    // Step 10: Send end-of-file (EOF) indicator to signal file transmission is complete
    const char *eof_message = "EOF";                          // End of file message
    send(client_socket, eof_message, strlen(eof_message), 0); // Send EOF message to the client

    // Step 11: Close the file and sockets after file transmission is done
    close(video_file_fd); // Close the file descriptor for the video file
    close(client_socket); // Close the client socket
    close(server_socket); // Close the listening socket

    // Notify that the file was sent and connection has been closed
    printf("File transfer completed successfully. Connection closed.\n");
    return 0; // Return success code
}
