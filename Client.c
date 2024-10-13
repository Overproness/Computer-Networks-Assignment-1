#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT_NUMBER 8877 // Server port number for connection
#define BUFFER_SIZE 1024 // Maximum buffer size for data transfer

int main()
{
    // Step 1: Define and initialize variables for socket setup
    struct sockaddr_in server_addr; // Structure to hold the server's address information
    int client_socket;              // Variable to store the client's socket descriptor

    // Zero out the server address structure and configure it
    memset(&server_addr, 0, sizeof(server_addr));    // Clear the memory for the server address
    server_addr.sin_family = AF_INET;                // Use IPv4 addressing
    server_addr.sin_port = htons(PORT_NUMBER);       // Set the port number (convert to network byte order)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Allow connections from any IP address (localhost)

    // Step 2: Create a socket for communication
    if ((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: Unable to create socket."); // Print error if socket creation fails
        return 1;
    }

    // Step 3: Establish connection to the server using the configured server address
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error: Connection to the server failed."); // Print error if connection attempt fails
        return 1;
    }

    // Step 4: Open a new file to save the received video data
    int video_file_fd = open("downloadedVideo.mp4", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (video_file_fd < 0)
    {
        perror("Error: Unable to create or open the video file for writing."); // Print error if file can't be opened
        close(client_socket);                                                  // Close the socket before exiting due to error
        return 1;
    }

    // Step 5: Prepare to receive data from the server
    char data_buffer[BUFFER_SIZE]; // Buffer to store incoming data
    ssize_t received_bytes;        // Variable to store the number of bytes received in each segment

    // Step 6: Loop to receive the video file in chunks
    while (true)
    {
        received_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0); // Receive data from server
        if (received_bytes < 0)
        {
            perror("Error: Problem occurred while receiving data."); // Print error on data reception failure
            break;
        }
        else if (received_bytes == 0)
        {
            // Server has closed the connection
            break;
        }

        // Step 7: Check if the server sent the end-of-file (EOF) indicator
        if (strncmp(data_buffer, "EOF", received_bytes) == 0)
        {
            printf("Notification: End of video file transmission detected.\n"); // Notify user that EOF was reached
            break;
        }

        // Step 8: Write the received chunk of data into the video file
        write(video_file_fd, data_buffer, received_bytes); // Save the received data to the file
    }

    // Step 9: Cleanup - Close the file and socket after data reception is complete
    close(video_file_fd); // Close the video file descriptor
    close(client_socket); // Close the socket connection to the server

    // Final notification to the user
    printf("Success: Video file has been successfully received, and the connection is now closed.\n");
    return 0; // Return success code
}
