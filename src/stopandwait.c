#include <stdio.h>
#include "file.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

// creating a data packet
typedef struct packet{
    char data[256];
}Packet;

// creating a packet with frame_kind (ACK, SEQ, FIN), data packet and packet size
typedef struct frame{
    int frame_kind; //ACK:0, SEQ:1 FIN:2
    int sq_no;
    int ack;
    int p_size;
    Packet packet;
}Frame;

void stopandwait_server(char* iface, long port, FILE* fp) {

    int sockfd;
    // socklen_t len;
    char buffer[256];
    int f_recv_size;
    // char some_string[1];
    int k = 0;
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

	int frame_id=0;
	Frame frame_recv;
	Frame frame_send;	
      
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("Socket Bind Failed");
        exit(EXIT_FAILURE);
    }

    socklen_t len = sizeof(cliaddr);

    while(1)
    {
            //recvfrom returns number of bytes from the IO stream
            f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(Frame), 0, ( struct sockaddr *) &cliaddr, &len);
            // printf("recvsize - %d \n", frame_recv.p_size);
            // printf("data - %s \n",frame_recv.packet.data);

            if(f_recv_size <= 0)
            {
                fclose(fp);
                close(sockfd);
                exit(0);
            }
            else
            {
                k = k + 1;

                //writing the frame to the file
                memcpy(buffer, frame_recv.packet.data, 256);
                // printf("data - %s \n",buffer);
                fwrite(buffer, frame_recv.p_size, 1, fp);
                
                frame_send.sq_no = 0;
                frame_send.frame_kind = 0;
                frame_send.ack = frame_recv.sq_no + 1;

                //sending the acknowledgement
                sendto(sockfd, &frame_send, sizeof(frame_send), 0, (struct sockaddr*) &cliaddr, len);

                printf("Packet [%d] Received \n", k);
                // perror("Receieved Failed");
            }
		    
            frame_id++;
    }
  
}

void stopandwait_client(char* host, long port, FILE* fp) {

    int sockfd;
	// socklen_t len;
    int k = 0, x, b;
    char buffer[256];
    int time_out;
    struct sockaddr_in     servaddr;
    int f_recv_size;

    // timeout
    struct timeval timeout;      
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { perror("Socket Creation Failed"); exit(EXIT_FAILURE); }
  
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

	int frame_id = 0;
	Frame frame_send;
	Frame frame_recv;

    // timeout thingy
    time_out = setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

    if(fp == NULL){ printf("\n Error in File Reading\n"); exit(0); }

    while(1)
    {
        k = k + 1;
        int ack_recv = 1;
        // printf("%d", ack_recv);
        
        if(ack_recv == 1)
        {   
            frame_send.sq_no = frame_id;
            frame_send.frame_kind = 1;
            frame_send.ack = 0;
        }
            while(!feof(fp))
            {        
                //fread returns number of bytes from the IO stream
                b = fread(buffer, 1, 256, fp);
                memcpy(frame_send.packet.data, buffer, 256);
                frame_send.p_size = b;
                // printf("MSG - %s %d \n", frame_send.packet.data, b);

                resend: x = sendto(sockfd, &frame_send, sizeof(frame_send), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
                if(x == -1){ perror("Error in File Transfer");}
                //frame send
                // printf("SENT");
            
                socklen_t len = sizeof(servaddr);
                f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(frame_recv), 0 ,(struct sockaddr*)&servaddr, &len);

                if(time_out <= 0.300)
                {
                    if( f_recv_size > 0 && frame_recv.sq_no == 0 && frame_recv.ack == frame_id+1)
                    {
                        printf("Acknowledge Receieved for Packet [%d] \n", k);
                        ack_recv = 1;
                    }
                    else
                    {
                        printf("Acknowledge Not Receieved for Packet [%d] \n", k);
                        // printf("%s \n", buffer);
                        ack_recv = 0;
                        goto resend;
                    }
                }
                else
                {
                        printf("Acknowledge Never Receieved for Packet [%d] \n", k);
                        ack_recv = 0;
                        goto resend;
                }
                k = k + 1;
            }
            // frame_send.end = -1;
            // memset(buffer, 0, sizeof(buffer));
            ack_recv = 0;
            sendto(sockfd, 0, 0, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
            bzero(buffer, sizeof(buffer));
            // frame_id++;
            // printf("kk");
            memset(buffer, 0, sizeof(buffer));
            close(sockfd);
            exit(0);
        
    }
}