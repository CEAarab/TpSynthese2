#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s host file\n", argv[0]);
        return 1;
    }

    char *host = argv[1];
    char *file = argv[2];

    // Initialize address info hints
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints)); // Clear the structure
    hints.ai_family = AF_INET; // Use IPv4
    hints.ai_socktype = SOCK_DGRAM; // Datagram socket

    // Get address information corresponding to the hostname
    int status = getaddrinfo(host, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }

    char ip[INET_ADDRSTRLEN]; // Buffer to store IP address
    void *addr;
    // Iterate through the linked list of address structures
    for (p = res; p != NULL; p = p->ai_next) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr = &(ipv4->sin_addr); // Get the address from the structure
        inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN); // Convert binary address to presentation form
    }

    printf("IP Address of %s is %s\n", host, ip); // Print the resolved IP address

    freeaddrinfo(res); // Free the memory allocated for address info

    return 0;
}

