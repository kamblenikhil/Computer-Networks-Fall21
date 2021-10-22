#include "file.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
/*
 *  Here is the starting point for your netster part.2 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */

void tcp_file_server(long port, FILE *fp)
{
    int server_fd = 0;
    // int server_socket= 0;
    int client_socket= 0;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[255] = {0};

    //added
    char *filename = "file2.txt";
    fp = fopen(filename, "w");

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { perror("Socket Creation Failed"); exit(0); }
       
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ perror("setsockopt Error"); exit(0); }

    memset(&address, 0, sizeof(address));
    
    //ip address and port number assignment
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
       
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){ perror("Socket Bind Failed Error"); exit(0); }
    
    if (listen(server_fd, 3) < 0){ perror("Socket Listen Failed"); exit(0); }

    while(1)
    {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){ perror("Socket Accept Failed"); exit(0); }
        else {

                while(1)
                {
                    //added
                    fprintf(fp, "%s", buffer);

                }
                memset(buffer, 0, sizeof(buffer));
            }
        }
        memset(buffer, 0, sizeof(buffer));
        close(client_socket);
    memset(buffer, 0, sizeof(buffer));

}

void tcp_file_client(long port, FILE *fp)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[255] = {0};
    
    //added
    char *filename = "file1.txt";
    fp = fopen(filename, "r");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ printf("\n Socket creation error \n"); exit(0); }
   
    memset(&serv_addr, 0, sizeof(serv_addr));
    
    //ip address and port number assignment
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){ printf("\n IP Address not valid \n"); exit(0); }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ printf("\n Socket Failed Connection\n"); exit(0); }

    while(1)
    {
        //to get input from the client terminal
        if(fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            send(sock, buffer, sizeof(buffer), 0);
        }

        memset(buffer, 0, sizeof(buffer));

    }
    close(sock);
}

/*
void udp_file_server(long port){
    int sockfd;
    char buffer[1024] = {0};
    char str[100];
    struct sockaddr_in servaddr, cliaddr;
      
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Socket Creation Failed");
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
        perror("Socket Bind Failed");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        unsigned int len = sizeof(cliaddr);
        recvfrom(sockfd, (char *)buffer, sizeof(buffer), MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
        printf("got message from ('%s', %d)\n", inet_ntop(AF_INET,&cliaddr.sin_addr,str,sizeof(str)), htons(cliaddr.sin_port));


        // if client message contains "hello" then server responds with "world".
        if (strncmp("hello", buffer, strlen("hello")) == 0){ 
            sendto(sockfd, "world\n", strlen("world\n"), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
        }

        // if client message contains "goodbye" then server responds with "farewell".
        else if (strncmp("goodbye", buffer, strlen("goodbye")) == 0){
            sendto(sockfd, "farewell\n", strlen("farewell\n"), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
        }
        
        // if msg contains "exit" then server exit and chat ended.
        else if (strncmp("exit", buffer, strlen("exit")) == 0){
            sendto(sockfd, "ok\n", strlen("ok\n"), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
            close(sockfd);
            break;
        }
        
        else{
            // print the message from client
            sendto(sockfd, buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
        }
        
        memset(buffer, 0, sizeof(buffer));
    }
    memset(buffer, 0, sizeof(buffer));

}

void udp_file_client(long port){
    int sockfd;
    char buffer[1024] = {0};
    struct sockaddr_in     servaddr;
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Socket Creation Failed");
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

            //if the client send exit, it should print the reply from server(ie. ok) and exit
            if ((strncmp(buffer, "exit", strlen("exit"))) == 0)
            {
                memset(buffer, 0, sizeof(buffer));
                //print the incoming message from the server
                recvfrom(sockfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
                printf("%s", buffer);
                exit(0);
            }
        }
        memset(buffer, 0, sizeof(buffer));
        recvfrom(sockfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
        printf("%s", buffer);

        memset(buffer, 0, sizeof(buffer));
    }
    memset(buffer, 0, sizeof(buffer));
    close(sockfd);
}
*/

void file_server(char* iface, long port, int use_udp, FILE* fp) 
{
    printf("connection ");
    if(use_udp)
    {
        printf("udp server");
        // udp_file_server(port, fp);
    }
    else
    {
        // printf("tcp server");
        tcp_file_server(port, fp);
    }  
}

void file_client(char* host, long port, int use_udp, FILE* fp) 
{
    if(use_udp)
    {
        printf("udp client");
        // udp_file_client(port, fp);
    }
    else
    {
        // printf("tcp client");
        tcp_file_client(port, fp);
    }
}
