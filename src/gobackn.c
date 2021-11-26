#include <stdio.h>
#include "file.h"

#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

// THIS PART IS USED FOR DATA PACKET
///////////////////////////////////////////////////////////////////////////////////////////////////////

// creating a data packet
typedef struct packet{
    char data[240];
}Packet;

// creating a packet with frame_kind (ACK, SEQ, FIN), data packet and packet size
typedef struct frame{
    int sq_no;
    int ack;
    int p_size;
    Packet packet;
}Frame;

///////////////////////////////////////////////////////////////////////////////////////////////////////

// THIS PART IS USED FOR RING BUFFER (TO MAINTAIN N PACKETS AT CLIENT/SERVER SIDE) - (Linked List Nodes)
///////////////////////////////////////////////////////////////////////////////////////////////////////

// node with sq_no, data and p_size
struct node_t {
    int sq_no;
    int p_size;
    char data[240];
    struct node_t* next;
};

// removing the first node_data from ring_buffer
int pop_first_element(struct node_t* head) 
{
    int sq_no = head->sq_no;
    int p_size = head->p_size;
    char data[240] = head->data;

    struct node_t* temp;

    if (head == NULL) {
        return -1;
    }

    // removing the first element
    temp = head;
    head = head->next;

    // release the memory for that first element
    free(temp);

    return sq_no, p_size, data;
}

// searching the elements from the ring buffer
int searching_the_element(struct node_t* head, int window_size)
{
    int sq_no = head->sq_no;
    int p_size = head->p_size;
    char data[240] = head->data;

    struct node_t* temp;

    if (head == NULL) {
        return -1;
    }

    // removing the first element
    temp = head;
    head = head->next;

    // release the memory for that first element
    free(temp);

    return sq_no, p_size, data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void gbn_server(char* iface, long port, FILE* fp) {

    int sockfd;
    // socklen_t len;
    char buffer[240];
    //int timeout;
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

    bzero(&servaddr.sin_zero, 8);
      
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

    //struct timeval timeout;
    //timeout.tv_sec = 1;
    //timeout.tv_usec = 70 * 1000;
    int xyz = 1;
    setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &xyz, sizeof(xyz));

    socklen_t len = sizeof(cliaddr);

    while(1)
    {
            f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(Frame), 0, ( struct sockaddr *) &cliaddr, &len);

            if(f_recv_size <= 0 || frame_recv.p_size == 0)
            {
                close(sockfd);
                return;
            }
            else
            {
                k = k + 1;

                //writing the frame to the file
                memcpy(buffer, frame_recv.packet.data, 240);

                if(frame_recv.p_size == 0)
                {
                    //fflush(fp);
                    close(sockfd);
                    return;
                }

                fwrite(buffer, frame_recv.p_size, 1, fp);
                
                frame_send.sq_no = 0;
                frame_send.ack = frame_recv.sq_no + 1;

                //sending the acknowledgement
                sendto(sockfd, &frame_send, sizeof(frame_send), 0, (struct sockaddr*) &cliaddr, len);

                printf("Packet [%d] Received \n", k);
		        memset(buffer, 0, sizeof(buffer));
            }
            frame_id++;
    }
}

void gbn_client(char* host, long port, FILE* fp) {

    int sockfd;
    int window_size = 5;
    int k = 0, x, b;
    char buffer[240];
    struct sockaddr_in     servaddr;
    int f_recv_size;

    // timeout
    struct timeval timeout;      
    timeout.tv_sec = 0;
    timeout.tv_usec = 50 * 1000;
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { perror("Socket Creation Failed"); exit(EXIT_FAILURE); }
  
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    struct hostent *hostname = gethostbyname(host);
    unsigned int server_address = *(unsigned long*) hostname->h_addr_list[0];
    servaddr.sin_addr.s_addr = server_address;

	int sequence_no = 0;
	Frame frame_send;
	Frame frame_recv;

    // initializing ring buffer Linked List (Node)
    struct node_t* head = NULL;
    head = (struct node_t*)malloc(sizeof(struct node_t));
    head->next = NULL;

    // timeout thingy
    setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    // adding chunks of data from file to ring buffer(Singly Linked List Implementation) 
    int count_seq = 0;
    while(!feof(fp))
    {
            // printf("oioi");
            struct node_t* temp = NULL;
            temp = (struct node_t*)malloc(sizeof(struct node_t));
            
            b = fread(buffer, 1, 240, fp);
            memcpy(temp->data, buffer, 240);
            temp->p_size = b;
            temp->sq_no = count_seq;

            if(count_seq == 0)
            {
                head = temp;
            }
            else
            {
                struct node_t* tp;
                tp = head;
                while(tp->next != NULL)
                {
                    tp = tp->next;
                }
                // printf("%s \t",tp->data);
                // printf("%d \t",tp->sq_no);
                tp->next = temp;
                // tp->next->next = NULL;
            }
            count_seq++;
    }
        // end of all node
        head->next = NULL;

    if(fp == NULL){ printf("\n Error in File Reading\n"); exit(0); }

    while(1)
    {
        k = k + 1;
        int ack_recv = 1;
        // // printf("%d", ack_recv);
        
        if(ack_recv == 1)
        {   
            frame_send.sq_no = sequence_no;
            frame_send.ack = 0;
        }

        // traversing the ring buffer - linked list
        struct node_t* some;
        while(some->next != NULL)
        {
            some = some->next;
        }

        frame_send.sq_no;
        frame_send.p_size;
        frame_send.packet.data;

        // fread returns number of bytes from the IO stream

        memcpy(frame_send.packet.data, buffer, 240);
        frame_send.p_size = b;
        printf("MSG - %s %d \n", frame_send.packet.data, b);

        resend: x = sendto(sockfd, &frame_send, sizeof(frame_send), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        if(x == -1){ perror("Error in File Transfer");}
        
        socklen_t len = sizeof(servaddr);
        f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(frame_recv), 0 ,(struct sockaddr*)&servaddr, &len);

        if( f_recv_size > 0 && frame_recv.sq_no == 0 && frame_recv.ack == sequence_no+1)
        {
            printf("Acknowledge Receieved for Packet [%d] \n", k);
		    memset(buffer, 0, sizeof(buffer));
            ack_recv = 1;
        }
        else if(f_recv_size == -1)
        {
            printf("Acknowledge Not Receieved for Packet [%d] \n", k);
            // printf("%s \n", buffer);
            ack_recv = 0;
            goto resend;
        }
        k = k + 1;
    }//end of while(1)

    memset(buffer, 0, sizeof(buffer));
    for(int i = 0; i < 7; i++)
	{
    	sendto(sockfd, 0, 0, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    }
    close(sockfd);
	return;
}