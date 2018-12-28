#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <tcpmgr_sock.h>

void interrupt_close(int arg);
void* serial_recv(void* arg);
void* serial_send(void* arg);

sock_t sockSvc;
pthread_t recvHandle;
pthread_t sendHandle;

int main(int argc, char* argv[])
{
    int iResult;
    int serverPort;
    char *serverIP, *tmpPtr;
    struct sockaddr_in addrInfo;

    signal(SIGINT, interrupt_close);

    // Checking
    if (argc < 3)
    {
        printf("test_connection <server_ip> <server_port>\n");
        return -1;
    }
    else
    {
        serverIP = argv[1];
        serverPort = strtol(argv[2], &tmpPtr, 10);
        if (tmpPtr == argv[2])
        {
            printf("Failed to convert %s to serverPort", argv[2]);
            return -1;
        }

        printf("Using serverIP: %s, serverPort: %d\n", argv[1], serverPort);
    }

    // Create socket
    sockSvc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockSvc < 0)
    {
        printf("Failed to create socket\n");
        return -1;
    }

    // Connect to server
    addrInfo.sin_family = AF_INET;
    addrInfo.sin_addr.s_addr = inet_addr(serverIP);
    addrInfo.sin_port = htons(serverPort);
    iResult =
        connect(sockSvc, (struct sockaddr*)&addrInfo, sizeof(struct sockaddr));
    if (iResult < 0)
    {
        printf("Failed to connect %s:%d\n", serverIP, serverPort);
        return -1;
    }

    // Create receive thread
    iResult = pthread_create(&recvHandle, NULL, serial_recv, NULL);
    if (iResult < 0)
    {
        printf("pthread_create() failed with error: %d\n", iResult);
        goto RET;
    }

    // Create send thread
    iResult = pthread_create(&sendHandle, NULL, serial_send, NULL);
    if (iResult < 0)
    {
        printf("pthread_create() failed with error: %d\n", iResult);
        goto RET;
    }

    // Join thread
    pthread_join(recvHandle, NULL);
    pthread_cancel(sendHandle);

RET:
    sock_close(sockSvc);

    return 0;
}

void* serial_recv(void* arg)
{
    int iResult;
    char readTmp = 0;
    while (1)
    {
        iResult = recv(sockSvc, &readTmp, 1, 0);
        if (iResult > 0)
        {
            printf("Receive: %c, 0x%X\n", readTmp, readTmp);
        }
        else
        {
            if (iResult == 0)
            {
                printf("Connection closed.\n");
            }
            else
            {
                perror("recv() failed with error");
            }

            pthread_exit(NULL);
            return NULL;
        }
    }

    return NULL;
}

void* serial_send(void* arg)
{
    int iResult;
    char sendTmp;
    while (1)
    {
        iResult = scanf("%c", &sendTmp);
        if (iResult > 0 && sendTmp >= ' ')
        {
            iResult = send(sockSvc, &sendTmp, 1, 0);
            if (iResult <= 0)
            {
                perror("send() failed with error");
                pthread_exit(NULL);
                return NULL;
            }
            else if (iResult > 0)
            {
                printf("Sent: %c\n", sendTmp);
            }
        }
    }

    return NULL;
}

void interrupt_close(int arg)
{
    pthread_cancel(recvHandle);
    pthread_cancel(sendHandle);

    sock_close(sockSvc);

    exit(0);
}
