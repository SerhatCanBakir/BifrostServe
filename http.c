#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#include "externalFuncs.h"
#define MAX_CLINETS 10
#define MAX_BUFF 2048

int main()
{
    // Gerekli tanımlamaalr
    WSADATA WSAData;
    SOCKET serverSocket, clientSocket;
    char buff[2048];
    struct sockaddr_in serveraddr, clientaddr;
    WSAPOLLFD pollfds[MAX_CLINETS];
    int clientCount = 0;
    int clientSize = sizeof(clientaddr);
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) == SOCKET_ERROR)
    {
        printf("WSAStartup doesnt work!!!");
        return 31;
    }

    // socket oluşturma
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == SOCKET_ERROR)
    {
        printf("serverSocket coudnt create");
        WSACleanup();
        return 31;
    }

    // server adresi ayarlama
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(3000);

    if (bind(serverSocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
    {
        printf("bind coudent work");
        closesocket(serverSocket);
        WSACleanup();
        return 31;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("listen coudent work");
        closesocket(serverSocket);
        WSACleanup();
        return 31;
    }

    printf("server is listening at 3000\n");

    pollfds[0].fd = serverSocket;
    pollfds[0].events = POLLIN;
    while (1)
    {
        int activity = WSAPoll(pollfds, clientCount + 1, -1);
        if (activity < 0)
        {
            printf("WSAPool ERR");
            break;
        }

        if (pollfds[0].revents & POLLIN)
        {
            clientSocket = accept(serverSocket, (struct sockaddr *)&clientaddr, &clientSize);
            if (clientSocket == INVALID_SOCKET)
            {
                printf("socket gecersiz");
                continue;
            }
            //?
            printf("Yeni istemci bağlandı: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
            if (clientCount < MAX_CLINETS)
            {
                clientCount++;
                pollfds[clientCount].fd = clientSocket;
                pollfds[clientCount].events = POLLIN;
            }
        }

        for (int i = 1; i <= clientCount; i++)
        {
            if (pollfds[i].revents & POLLIN)
            {
                memset(buff, 0, 2048);
                int bytesReceived = recv(pollfds[i].fd, buff, 2048, 0);

                if (bytesReceived <= 0)
                {
                    printf("istemci bağlantisi saglandi");
                    closesocket(pollfds[i].fd);
                    pollfds[i] = pollfds[clientCount];
                    clientCount--;
                }
                else
                {
                    buff[bytesReceived] = '\0';
                    struct req request;
                    int temp = copyUntilSpace(buff, request.method);
                    temp += copyUntilSpace(&buff[temp], request.url);
                    printf("%s", buff);
                    printf("%s \n %s",request.method,request.url);
                    int sa = strcmp(request.method, "GET");
                    if(sa==0)
                    {
                        struct response res;
                        res.status=200;
                        strcpy(res.body,readHtmlFile("./site.html"));
                        res.contentLenght=strlen(res.body)-1;
                        char* dest;
                        prepareResponse(&res,&dest);
                        printf("%s",dest);
                        send(pollfds[i].fd, dest, strlen(dest), 0);
                    }
                }
            }
        }
    }
}