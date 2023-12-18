#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 516

// Structure representing the TFTP Data Packet
struct TFTPDataPacket {
    uint16_t opcode; // TFTP opcode (2 bytes)
    uint16_t block_num; // Block number (2 bytes)
    char data[MAX_BUFFER_SIZE - 4]; // Data content (Maximum buffer size - 4 bytes for opcode and block number)
};

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s host file\n", argv[0]);
        return 1;
    }

    // Assign command line arguments to host and file variables
    char *host = argv[1];
    char *file = argv[2];

    // Initialize and set up address info hints
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 addresses
    hints.ai_socktype = SOCK_DGRAM; // Datagram socket for UDP
    hints.ai_protocol = IPPROTO_UDP;
    // Get address information corresponding to the provided host
    int status = getaddrinfo(host, "1069", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }

    int sockfd = -1;
    // Iterate through available address info results to create a socket
    if (res != NULL) {
        // Create a socket based on the address info
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            // If socket creation fails, set sockfd to 0
            sockfd = 0;
        }
    }

    // Check if socket creation failed
    if (sockfd == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return 3;
    }

    char request[MAX_BUFFER_SIZE];
    char buffer[MAX_BUFFER_SIZE];

    // Construct RRQ packet
    request[0] = 0x00; // Opcode - Read Request (RRQ)
    request[1] = 0x01; // Opcode - Read Request (RRQ)
    strcpy(&request[2], file); // Copy the filename to the request buffer
    strcpy(&request[strlen(file) + 3], "octet"); // Add transfer mode "octet"
    request[strlen(file) + 3 + strlen("octet") + 1] = 0x00; // End with null byte
    sendto(sockfd, request, strlen(file) + strlen("octet") + 4, 0, res->ai_addr, res->ai_addrlen);

    struct sockaddr server_addr;
    socklen_t addr_len = sizeof(server_addr);
    int block_num = 1;
    FILE *received_file = fopen(file, "wb");
    
    // Loop to receive data packets and write to the file
    do {
        struct TFTPDataPacket data_packet;
        ssize_t received_bytes = recvfrom(sockfd, &data_packet, MAX_BUFFER_SIZE, 0, &server_addr, &addr_len);

        // Check for error packet
        if (ntohs(data_packet.opcode) == 5) {
            fprintf(stderr, "Error received from server: %.*s\n", (int)(received_bytes - 4), data_packet.data);
            return 6;
        }

        // Process received data packet
        if (ntohs(data_packet.opcode) == 3) {
            if (ntohs(data_packet.block_num) == block_num) {
                // Write received data to the file
                fwrite(data_packet.data, 1, received_bytes - 4, received_file);

                // Prepare and send acknowledgment packet (ACK)
                char ack_packet[4];
                ack_packet[0] = 0;
                ack_packet[1] = 4; // Opcode - Acknowledgment (ACK)
                ack_packet[2] = data_packet.block_num >> 8; // Block number
                sendto(sockfd, ack_packet, sizeof(ack_packet), 0, &server_addr, addr_len);

                // Check if reached end of file transfer
                if (received_bytes < MAX_BUFFER_SIZE) {
                    break;
                }

                // Increment block number for the next packet
                block_num++;
            }
        } else if (ntohs(data_packet.opcode) != 4) {
            // Handle unknown opcode
            fprintf(stderr, "Received unknown opcode: %d\n", ntohs(data_packet.opcode));
            break;
        }
    } while (1); // Continue receiving until the entire file is received

    // Close the file, socket, and free address info memory
    fclose(received_file);
    close(sockfd);
    freeaddrinfo(res);

    return 0;
}

