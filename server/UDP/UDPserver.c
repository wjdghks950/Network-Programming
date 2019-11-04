#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void receiveMessage(int sockfd, int buf_size, struct sockaddr* client, int* client_len)
{
    char* buffer = (char*)malloc(buf_size);
    int received = 0;
    char * closemsg = "Server closed! Thank you.";

    while(1)
    {
        memset(buffer, 0, buf_size); // Initialize buffer contents to 0's
        received = recvfrom(sockfd, (char*)buffer, buf_size, 0, client, client_len);
        if (received < 0)
        {
            fprintf(stderr, "recvfrom() failed!\n");
            exit(0);
        }
        else if (received == 0)
        {
            printf("Server side closing...\n");
            close(sockfd);
        }
        else
        {
            buffer[received] = '\0';
            if(strlen(buffer) > 0)
            {
                printf("[Client] : %s\n", buffer);
                if(!strncmp(buffer, "q", 1))
                {
                    printf("Server side closing...\n");
                    close(sockfd);
                    break;
                }
            }
        }
    }

}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "provide port number and buffer size\n");
        exit(0);
    }

    int sockfd, client_len;
    int portnum = atoi(argv[1]);
    int buf_size = atoi(argv[2]);
    struct sockaddr_in server_addr, client;
    memset(&server_addr, 0, sizeof(server_addr)); // Clear address buffer with 0's
    memset(&client, 0, sizeof(client));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    printf("[Port Number]: %d\n", portnum);
    printf("[Buffer Size]: %d\n", buf_size);

    client_len = sizeof(client);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) // SOCK_DGRAM for UDP connection
    {
        perror("socket error\n");
        exit(0);
    }
    else
    {
        printf("Socket created...\n");
    }

    if ((bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) != 0)
    {
        perror("socket bind error");
        exit(0);
    }
    else
    {
        printf("Socket binding success...\n");
    }

    /* Wait until datagram packet arrives from client */
    sleep(5);
    receiveMessage(sockfd, buf_size, (struct sockaddr*)&client, &client_len);

    return 0;
}
