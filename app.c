#include "app.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#define MAX_CLINETS 20
#define MAX_BUFF 2048

// bu fonksiyon appi başlangıç duruma getirir ve gerekli bellek alanlarını ayırır
int appInit(APP *app)
{
    app->resSize = 0;
    app->reqSize = 0;

    for (int i = 0; i < 50; i++)
    {
        app->req[i].method = (char *)malloc(16);
        app->req[i].url = (char *)malloc(256);
        app->req[i].headers = (char *)malloc(512);
        app->req[i].body = (char *)malloc(2048);

        app->res[i].contentType = (char *)malloc(64);
        app->res[i].body = (char *)malloc(2048);
    }

    return 0;
}

// bu fonksiyon bellekte yaratılan bütün adresleri serbest bırakır
void appDestroy(APP *app)
{
    for (int i = 0; i < app->reqSize; i++)
    {
        free(app->req[i].method);
        free(app->req[i].url);
        free(app->req[i].headers);
        free(app->req[i].body);
    }

    for (int i = 0; i < app->resSize; i++)
    {
        free(app->res[i].contentType);
        free(app->res[i].body);
    }

    printf("Bellek başarıyla temizlendi.\n");
}

// bu fonksiyon app structına gelen istek ve karşılığında gönderilicek sonucu ekler
int appendrequest(APP *app, struct request req, struct response res)
{
    if (app->reqSize >= 50 || app->resSize >= 50)
    {
        return -1;
    }

    app->req[app->reqSize].method = strdup(req.method);
    app->req[app->reqSize].url = strdup(req.url);
    app->req[app->reqSize].headers = strdup(req.headers);
    app->req[app->reqSize].body = strdup(req.body);

    app->res[app->resSize].contentType = strdup(res.contentType);
    app->res[app->resSize].body = strdup(res.body);
    app->res[app->resSize].status = res.status;
    app->res[app->resSize].contentLenght = res.contentLenght;

    app->reqSize++;
    app->resSize++;

    return 0;
}
// bu fonksiyon gelen isteği appteki isteklerle karşılaştırarak aynıysa response cevapı yazar
int checkReq(APP *app, char *comingRequest, char **returnResp)
{
    char method[16];
    char url[256];
    int temp = copyUntilSpace(comingRequest, method);
    temp = copyUntilSpace(&comingRequest[temp + 1], url);
    for (int i = 0; i <= app->reqSize; i++)
    {
        if ((strcmp(app->req[i].method, method) == 0) && (strcmp(app->req[i].url, url) == 0))
        {
            struct response res = app->res[i];
            prepareResponse(&res, returnResp);
            return 0;
        }
    }
    return -1;
}

// bu fonksiyon malloc kullanarak request structını yaratmaya yarar
struct request *createRequest(char *method, char *url, char *header, char *body)
{
    struct request *req = (struct request *)malloc(sizeof(struct request));
    req->method = (char *)malloc(16);
    req->url = (char *)malloc(256);
    req->headers = (char *)malloc(512);
    req->body = (char *)malloc(2048);

    if (!req->method || !req->url || !req->headers || !req->body)
    {
        printf("HATA: Bellek tahsis edilemedi!\n");
        return NULL;
    }

    strcpy(req->method, method);
    strcpy(req->url, url);
    strcpy(req->headers, header);
    strcpy(req->body, body);
    return req;
}

// bı fonksiyon malloc kullanarak response structını yaratmaya yarar
struct response *createResponse(int status, char *contentType, char *body)
{
    struct response *res = (struct response *)malloc(sizeof(struct response));
    res->status = (int)malloc(sizeof(int));
    res->contentLenght = (int)malloc(sizeof(int));
    res->contentType = (char *)malloc(512);
    res->body = (char *)malloc(2048);

    if (!res->body || !res->status || !res->contentLenght || !res->contentType)
    {
        printf("HATA: Bellek yetersiz git ram al ");
        return NULL;
    }
    res->status = status;
    res->contentLenght = strlen(body);
    strcpy(res->body, body);
    strcpy(res->contentType, contentType);
    return res;
}

// isteğin değerlerini birbirinden ayırmaya yarar
int copyUntilSpace(const char *src, char *dest)
{
    int i = 0;
    while (src[i] != '\r' && src[i] != ' ')
    { // Boşluk veya null karaktere kadar ilerle
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; // Null terminator ekle
    return i;
}
// response structına göre cevap texti hazırlar
void prepareResponse(struct response *src, char **dest)
{
    *dest = (char *)malloc(512 + src->contentLenght); // Bellek ayır (eski 200 bayt yeterli değil!)
    if (*dest == NULL)
    {
        printf("Bellek tahsisi başarısız!\n");
        return;
    }

    sprintf(*dest, "HTTP/1.1 %d OK \r\nContent-Type: text/html \r\nContent-Length: %d \r\nConnection: close \r\n\r\n%s",
            src->status, src->contentLenght, src->body);
}
// dosya okumayı kolaylaştırır (html js css vb)
char *readFile(const char *filename)
{
    FILE *file = fopen(filename, "rb"); // Dosyayı okuma modunda aç (binary moduyla)
    if (!file)
    {
        printf("Dosya açılamadı!\n");
        return NULL;
    }

    // Dosya boyutunu öğren
    fseek(file, 0, SEEK_END);    // Dosya sonuna git
    long fileSize = ftell(file); // Konumu al (dosyanın toplam uzunluğu)
    rewind(file);                // Dosyanın başına dön

    // Bellek tahsis et (+1 çünkü string için sonuna '\0' ekleyeceğiz)
    char *buffer = (char *)malloc(fileSize + 1);
    if (!buffer)
    {
        printf("Bellek tahsisi başarısız!\n");
        fclose(file);
        return NULL;
    }

    // Dosyanın tamamını oku
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0'; // String'in sonuna '\0' koy

    fclose(file);  // Dosyayı kapat
    return buffer; // Pointer'ı döndür
}

// serverı başlatır tcp socketi açarak girilen ip ve porta bağlar ve WSAPOLLDF ile istekler birbirini blocklamadan cevap yollar
int startServer(APP *app, char *ipAddr, int PORT)
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
    if (serverSocket == INVALID_SOCKET)
    {
        printf("serverSocket coudnt create");
        WSACleanup();
        return 31;
    }

    // server adresi ayarlama
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(ipAddr);
    serveraddr.sin_port = htons(PORT);

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

    printf("server is listening at %d\n", PORT);

    pollfds[0].fd = serverSocket;
    pollfds[0].events = POLLIN;
    while (1)
    {
        int activity = WSAPoll(pollfds, (clientCount > 0) ? clientCount + 1 : 1, 5000);
        if (activity < 0)
        {
            printf("WSAPoll hatası! Hata kodu: %d\n", WSAGetLastError());
            break;
        }

        if (activity == 0)
        {
            printf("WSAPoll zaman aşımı, tekrar bekleniyor...\n");
            continue;
        }

        if (pollfds[0].revents & POLLIN)
        {
            clientSocket = accept(serverSocket, (struct sockaddr *)&clientaddr, &clientSize);
            if (clientSocket == INVALID_SOCKET)
            {
                printf("Accept başarısız! Hata kodu: %d\n", WSAGetLastError());
                continue;
            }

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
                memset(buff, 0, MAX_BUFF);
                int bytesReceived = recv(pollfds[i].fd, buff, MAX_BUFF, 0);

                if (bytesReceived <= 0)
                {
                    printf("İstemci bağlantısı kesildi.\n");
                    closesocket(pollfds[i].fd);

                    if (i != clientCount)
                    {
                        memmove(&pollfds[i], &pollfds[i + 1], (clientCount - i) * sizeof(WSAPOLLFD));
                    }

                    clientCount--;
                    continue;
                }
                else
                {
                    buff[bytesReceived] = '\0';

                    char *dest = NULL;
                    if (checkReq(app, buff, &dest) == 0 && dest != NULL)
                    {

                        send(pollfds[i].fd, dest, strlen(dest), 0);
                        free(dest);
                    }
                    else
                    {
                        printf("Bilinmeyen istek alındı.\n");
                    }
                }
            }
        }
    }
}
