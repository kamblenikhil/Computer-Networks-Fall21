/*
 *  Here is the starting point for your netster part.1 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define max 80

void tcp_chat_server(long port)
{
    int server_fd = 0;
    int new_socket= 0;
    struct sockaddr_in address;
    int opt = 1;
    int connection_counter = 0;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char str[100];

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { perror("socket failed"); exit(0); }
       
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ perror("setsockopt"); exit(0); }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
       
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){ perror("bind failed"); exit(0); }
    
    if (listen(server_fd, 3) < 0){ perror("listen"); exit(0); }
    
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){ perror("accept"); exit(0); }
    else {
        printf("connection %d from ('%s', %d)\n", connection_counter, inet_ntop(AF_INET,&address.sin_addr,str,sizeof(str)), htons(address.sin_port));
        connection_counter++;
    }

    while(1)
    {
        recv( new_socket , buffer, sizeof(buffer), 0);

        // if client message contains "hello" then server responds with "world".
        if (strncmp("hello", buffer, strlen("hello")) == 0){ 
            printf("got message from ('%s', %d)\n", inet_ntop(AF_INET,&address.sin_addr,str,sizeof(str)), htons(address.sin_port));
            send(new_socket, "world\n", strlen("world\n"), 0); 
            bzero(buffer, sizeof(buffer));
        }

        // if client message contains "goodbye" then server responds with "farewell".
        else if (strncmp("goodbye", buffer, strlen("goodbye")) == 0){
            printf("got message from ('%s', %d)\n", inet_ntop(AF_INET,&address.sin_addr,str,sizeof(str)), htons(address.sin_port));
            send(new_socket, "farewell\n", strlen("farewel\n"), 0);
            bzero(buffer, sizeof(buffer));
        }

        // if msg contains "exit" then server and client exit and chat is ended.
        else if (strncmp("exit", buffer, strlen("exit")) == 0){
            printf("got message from ('%s', %d)\n", inet_ntop(AF_INET,&address.sin_addr,str,sizeof(str)), htons(address.sin_port));
            send(new_socket, "ok\n", strlen("ok\n"), 0);
            bzero(buffer, sizeof(buffer));
            break;
        }
        else{
            // if any other message comes from client
            printf("got message from ('%s', %d)\n", inet_ntop(AF_INET,&address.sin_addr,str,sizeof(str)), htons(address.sin_port));
            send(new_socket, buffer, sizeof(buffer), 0);
            bzero(buffer, sizeof(buffer));
        }
    }
    close(new_socket);
}

void tcp_chat_client(long port)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ printf("\n Socket creation error \n"); exit(0); }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){ printf("\nInvalid address/ Address not supported \n"); exit(0); }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ printf("\nConnection Failed \n"); exit(0); }

    while(1)
    {
        if(fgets(buffer, sizeof(buffer), stdin) != 0){
            send(sock, buffer, sizeof(buffer), 0);
            bzero(buffer, sizeof(buffer));
        }

        //print the incoming message from the server
        recv(sock, buffer, sizeof(buffer), 0);
        printf("%s", buffer);

        //if server sends farewell/ok message, client terminal exits
        if ((strncmp(buffer, "farewell", strlen("farewell"))) == 0){ break; }
        if ((strncmp(buffer, "ok", strlen("ok"))) == 0){ break; }
    }
    close(sock);
}


void udp_chat_server(long port){
    int sockfd;
    char buffer[1024] = {0};
    char str[100];
    struct sockaddr_in servaddr, cliaddr;
      
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
      
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
      
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
      
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        unsigned int len = sizeof(cliaddr);
        recvfrom(sockfd, (char *)buffer, sizeof(buffer), MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);

        // if client message contains "hello" then server responds with "world".
        if (strncmp("hello", buffer, strlen("hello")) == 0){ 
            printf("got message from ('%s', %d)\n", inet_ntop(AF_INET,&cliaddr.sin_addr,str,sizeof(str)), htons(cliaddr.sin_port));
            sendto(sockfd, "world\n", strlen("world\n"), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
            bzero(buffer, sizeof(buffer));
        }

        // if client message contains "goodbye" then server responds with "farewell".
        else if (strncmp("goodbye", buffer, strlen("goodbye")) == 0){
            printf("got message from ('%s', %d)\n", inet_ntop(AF_INET,&cliaddr.sin_addr,str,sizeof(str)), htons(cliaddr.sin_port));
            sendto(sockfd, "farewell\n", strlen("farewell\n"), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
            bzero(buffer, sizeof(buffer));
        }
        
        // if msg contains "exit" then server exit and chat ended.
        else if (strncmp("exit", buffer, strlen("exit")) == 0){printf("Client Exit\n");}
        
        else{
            // print the message from client
            printf("got message from ('%s', %d)\n", inet_ntop(AF_INET,&cliaddr.sin_addr,str,sizeof(str)), htons(cliaddr.sin_port));
            sendto(sockfd, buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
            bzero(buffer, sizeof(buffer));
        }
    }

}

void udp_chat_client(long port){
    int sockfd;
    char buffer[1024] = {0};
    struct sockaddr_in     servaddr;
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
  
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    while(1)
    {
        unsigned int len = sizeof(servaddr);
       
        if(fgets(buffer, sizeof(buffer), stdin) != 0){
            sendto(sockfd, buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        }

        recvfrom(sockfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
        printf("%s", buffer);
        bzero(buffer, sizeof(buffer));
    }
    close(sockfd);
}



/* Add function definitions */
void chat_server(char* iface, long port, int use_udp) 
{
    // printf("connection ");
    if(use_udp)
    {
        // printf("udp server");
        udp_chat_server(port);
    }
    else
    {
        // printf("tcp server");
        tcp_chat_server(port);
    }
}

void chat_client(char* host, long port, int use_udp) 
{
    if(use_udp)
    {
        // printf("udp client");
        udp_chat_client(port);
    }
    else
    {
        // printf("tcp client");
        tcp_chat_client(port);
    }
}
