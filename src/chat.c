/*
 *  Here is the starting point for your netster part.1 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define max 80

/* Add function definitions */
void chat_server(char* iface, long port, int use_udp) {
    int server_fd = 0;
    int new_socket= 0;
    int n;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
       
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(0);
    }
       
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(0);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
       
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(0);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(0);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(0);
    }
    while(1)
    {
        bzero( buffer, max);

        int a = read( new_socket , buffer, sizeof(buffer));
        
        //print from client
        printf("%s", buffer);
        
        bzero(buffer, max);
        n = 0;

        // copy server message in the buffer
        while ((buffer[n++] = getchar()) != '\n');

        int b = write(new_socket, buffer, sizeof(buffer));
   
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buffer, 4) == 0) 
        {
            printf("Server Exit...\n");
            break;
        }

        if (a == b){}

    }
    close(new_socket);
}

void chat_client(char* host, long port, int use_udp) {
    int sock = 0;
    int n;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        exit(0);
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(0);
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        exit(0);
    }

    while(1)
    {
        bzero(buffer, sizeof(buffer));
        
        n = 0;
        while ((buffer[n++] = getchar()) != '\n');
        
        int a = write(sock, buffer, sizeof(buffer));

        bzero(buffer, sizeof(buffer));
        
        int b = read(sock, buffer, sizeof(buffer));
        
        //print from server
        printf("%s", buffer);
        
        if ((strncmp(buffer, "exit", 4)) == 0) 
        {
            printf("Client Exit...\n");
            break;
        }

        if(a == b) {}

    }
    close(sock);
}