#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define REPEAT 3
#define INITBYTE 10
#define MAXBUF 80
#define pLoss 0.5

int sendMessage(int sockfd, char * sndmsg, struct sockaddr_in* servaddr, int serv_len)
{
    char msg[80];
    memset(msg, 0, sizeof(msg));
    int i;
    for(i = 0; i < REPEAT; i++)
    {
        sprintf(msg, "%d", i+1);
        sndmsg[strlen(sndmsg)-1] = msg[0];
        printf("Msg sent: %s\n", sndmsg); 
        if (sendto(sockfd, (const char*)sndmsg, INITBYTE, 0, (const struct sockaddr*)servaddr, serv_len) < 0)
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
            double prob = 1/(1-pLoss) - 1; # Num. of packet resends
            int probL = (int)prob;
            for(i=0; i<probL; i++) # Resend
            {
                if (sendto(sockfd, "q", MAXBUF, 0, (const struct sockaddr*)servaddr, serv_len) < 0)
                {
                    perror("send() failed!\n");
                    exit(0);
                }
            }
            shutdown(sockfd, SHUT_WR); // Notify the server the end of file transmission
            break;
        }
        else
        {
            if (sendto(sockfd, (const char*)msg, MAXBUF, 0, (const struct sockaddr*)servaddr, serv_len) < 0)
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

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) // Create a UDP socket
    {
        perror("socket error\n");
        exit(0);
    }
    else
    {
        printf("Socket created...\n");
    }

    sendMessage(sockfd, firstMsg, &my_addr, sizeof(my_addr));

    return 0;
}

