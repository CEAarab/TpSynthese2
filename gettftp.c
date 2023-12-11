#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>


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

    // Get address information corresponding to the provided host
    int status = getaddrinfo(host, NULL, &hints, &res);
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
    memset(request, 0, MAX_BUFFER_SIZE);
    request[0] = 0x00;
    request[1] = 0x01;
    strcpy(&request[2], file);
    request[strlen(file) + 3] = 0x00;
    strcpy(&request[strlen(file) + 4], "octet");
    request[strlen(file) + 5 + strlen("octet")] = 0x00;
    
    struct sockaddr *server_addr = res->ai_addr;
    socklen_t addr_len = res->ai_addrlen;
    sendto(sockfd, request, strlen(file) + strlen("octet") + 5, 0, server_addr, addr_len);



    // Free memory allocated for address info
    freeaddrinfo(res);
    // Close Socket
    close(sockfd);

    return 0; 
}

