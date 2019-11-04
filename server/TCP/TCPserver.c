#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void receiveMessage(int sockfd, int buf_size)
{
    char* buffer = (char*)malloc(buf_size);
    int received = 0;
    char * closemsg = "Server closed! Thank you.";

    while(1)
    {
        memset(buffer, 0, buf_size); // Initialize buffer contents to 0's
        received = recv(sockfd, buffer, buf_size, 0);
        if (received < 0)
        {
            fprintf(stderr, "recv() failed!\n");
            exit(0);
        }
        else if (received == 0)
        {
            printf("Server side closing...\n");
            close(sockfd);
            break;
        }
        else
        {
            if(strlen(buffer) > 0)
            {
                printf("[Client] : %s\n", buffer);
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

    int sockfd, connectfd, client_len;
    int portnum = atoi(argv[1]);
    int buf_size = atoi(argv[2]);
    struct sockaddr_in server_addr, client;
    memset(&server_addr, 0, sizeof(server_addr)); // Clear address buffer with 0's

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    printf("[Port Number]: %d\n", portnum);
    printf("[Buffer Size]: %d\n", buf_size);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

    if (listen(sockfd, 5) != 0)
    {
        perror("listen error");
        exit(0);
    }
    else
    {
        printf("Server listening...\n");
    }

    /* Accept client - data packet from client */
    if ((connectfd = accept(sockfd, (struct sockaddr*)&client, &client_len)) < 0)
    {
        perror("server accept error\n");
        exit(0);
    }
    else
    {
        printf("Server accepted the client...\n");
        sleep(5); // Sleep for 5 seconds
    }

    receiveMessage(connectfd, buf_size);

    return 0;
}
