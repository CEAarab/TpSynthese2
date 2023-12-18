#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 516

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
    struct addrinfo hints, *res, *p;
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
    
    // Construct RRQ packet
    request[0] = 0x00; // Opcode - Read Request (WRQ)
    request[1] = 0x02; // Opcode - Read Request (WRQ)
    strcpy(&request[2], file); // Copy the filename to the request buffer
    strcpy(&request[strlen(file) + 3], "octet"); // Add transfer mode "octet"
    request[strlen(file) + 3 + strlen("octet") + 1] = 0x00; // End with null byte
    sendto(sockfd, request, strlen(file) + strlen("octet") + 4, 0, res->ai_addr, res->ai_addrlen);
    
    // Free memory allocated for address info
    freeaddrinfo(res);
    // Close Socket
    close(sockfd);

    return 0; 
}

