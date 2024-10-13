
# Video File Transfer using TCP Sockets

## Description

This repository contains two C programs that demonstrate how to transfer a video file over a network using TCP sockets. The project includes:

Client Program: Receives a video file from the server and saves it locally.
Server Program: Sends a video file to the client.
Both programs communicate over a predefined port and utilize basic file I/O operations to handle the video file transmission. The transfer is completed when the server sends an "EOF" (End of File) marker to the client, indicating the end of the file.

# Client Program: Video Receiver

## Approach
The client program connects to the server, receives the video file in segments, and writes the data to a local file. It maintains a persistent TCP connection to handle the data transfer, ensuring that the file is fully received and saved before the connection is closed.

## Design
### 1. Socket Setup:

The client creates a TCP socket using the socket() function.
It connects to the server using connect() with a predefined port (PORT_NUMBER).
### 2. File Creation:

After successfully connecting, the client opens a local file ("downloadedVideo.mp4") to write the incoming video data.
### 3. Receiving Data:

The client receives data from the server in chunks of a fixed buffer size (BUFFER_SIZE) using the recv() function.
Each received chunk is written into the local video file.
### 4. End-of-File Handling:

The client looks for an "EOF" marker in the received data to indicate the end of the file transfer.
### 5. Cleanup:

The client closes both the file and socket after the transfer is complete.
### Client Code Example:

~~~
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT_NUMBER 8877               // Server port number
#define BUFFER_SIZE 1024               // Buffer size for data transfer

int main() {
    struct sockaddr_in server_addr;
    int client_socket;

    // Socket setup and server connection
    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Open file for writing received video
    int video_file_fd = open("downloadedVideo.mp4", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    // Receiving and writing data
    char data_buffer[BUFFER_SIZE];
    ssize_t received_bytes;

    while (true) {
        received_bytes = recv(client_socket, data_buffer, sizeof(data_buffer), 0);
        if (strncmp(data_buffer, "EOF", received_bytes) == 0) {
            break;
        }
        write(video_file_fd, data_buffer, received_bytes);
    }

    // Cleanup
    close(video_file_fd);
    close(client_socket);

    printf("File received and connection closed.\n");
    return 0;
}
~~~


# Server Program: Video Sender
## Approach
The server program listens for incoming connections from clients, reads a video file from the local system, and sends it to the client in chunks. The connection is maintained until the file is completely sent, and an "EOF" marker is transmitted to signal the end of the file transfer.

### Design
### 1. Socket Setup:

The server creates a TCP socket using socket() and binds it to a specific port (PORT_NUMBER).
The server listens for incoming connections using listen().
### 2. Accepting Client Connection:

Once a client tries to connect, the server accepts the connection using accept().
The client’s IP address is displayed upon successful connection.
### 3. File Handling:

The server opens a local video file ("sampleVideo.mp4") in read-only mode.
### 4. Data Transmission:

The server reads the file in chunks using read() and sends each chunk to the client using send().
### 5. End-of-File Handling:

After the entire file is transmitted, the server sends an "EOF" marker to signal the end of the transfer.
### 6. Cleanup:

The server closes both the file descriptor and the socket after the transmission is complete.
## Server Code Example:
c
```
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT_NUMBER 8877               // Port for the server to listen on
#define BUFFER_SIZE 1024               // Buffer size for file transfer

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int server_socket, client_socket;

    // Socket setup
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUMBER);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 16);

    // Accept client connection
    printf("Waiting for a client to connect...\n");
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    printf("Client connected with IP address: %s\n", inet_ntoa(client_addr.sin_addr));

    // Open video file for reading
    int video_file_fd = open("sampleVideo.mp4", O_RDONLY);

    // Sending data in chunks
    char data_buffer[BUFFER_SIZE];
    ssize_t read_bytes;
    while ((read_bytes = read(video_file_fd, data_buffer, sizeof(data_buffer))) > 0) {
        send(client_socket, data_buffer, read_bytes, 0);
    }

    // Send EOF to client
    const char *eof_message = "EOF";
    send(client_socket, eof_message, strlen(eof_message), 0);

    // Cleanup
    close(video_file_fd);
    close(client_socket);
    close(server_socket);

    printf("File sent and connection closed.\n");
    return 0;
}
```

## Summary of Key Differences
Aspect	Client (Video Receiver)	Server (Video Sender)
Role	Receives video file from server	Sends video file to client
File I/O	Writes received data to a file	Reads video file from disk
EOF Handling	Detects "EOF" marker to end transfer	Sends "EOF" marker to signal end of file
Socket Action	Connects to the server	Listens and accepts connections from client
How to Run
Server:
Compile the server code:
bash
Copy code
gcc -o server server.c
Run the server:
bash
Copy code
./server
Client:
Compile the client code:
bash
Copy code
gcc -o client client.c
Run the client:
bash
Copy code
./client
Requirements
A Linux environment with GCC.
Network access between the client and server machines (if run on separate devices).
