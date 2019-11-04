#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define REPEAT 3
#define INITBYTE 10
#define MAXBUF 80

int sendMessage(int sockfd, char * sndmsg)
{
    char msg[80];
    memset(msg, 0, sizeof(msg));
    int i;
    for(i = 0; i < REPEAT; i++)
    {
        sprintf(msg, "%d", i+1);
        sndmsg[strlen(sndmsg)-1] = msg[0];
        printf("Msg sent: %s\n", sndmsg); 
        if (send(sockfd, sndmsg, INITBYTE, 0) < 0)
        {
            perror("send() failed!\n");
            exit(0);
        }
    }
    while(1)
    {
        printf("[Client] (press q to terminate): ");
        memset(msg, 0, sizeof(msg));
        fgets(msg, sizeof(msg), stdin);
        msg[strlen(msg)-1] = '\0';
        if(!strncmp(msg, "q", 1))
        {
            printf("close connection...\n");
            close(sockfd); // Notify the server the end of file transmission
            break;
        }
        else
        {
            if (send(sockfd, msg, MAXBUF, 0) < 0)
            {
                perror("send() failed!\n");
                exit(0);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "provide server_IP and server port number\n");
        exit(0);
    }
    int sockfd;
    int portnum = atoi(argv[2]);
    char firstMsg[11] = "HELLOMAN!0";
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr)); // Clear address buffer with 0's

    printf("Server IP: [%s]\n", argv[1]);
    printf("Server Port Number: [%s]\n", argv[2]);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(portnum);
    inet_aton(argv[1], &(my_addr.sin_addr)); /* my_addr.sin_addr.s_addr = inet_addr("203.252.106.83"); */

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // Create a socket
    {
        perror("socket error\n");
        exit(0);
    }
    else
    {
        printf("Socket created...\n");
    }

    if (connect(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) != 0)
    {
        perror("connection to the server: failed\n");
        exit(0);
    }
    else
    {
        printf("Connected to server at %s\n", argv[2]);
        sendMessage(sockfd, firstMsg);
    }

    return 0;
}

