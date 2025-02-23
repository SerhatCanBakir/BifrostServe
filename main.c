#include "app.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    APP myApp;
    appInit(&myApp);
    printf("app init calisti\n");

    struct request req = *createRequest("GET", "/", " ", " ");
    printf("req hazırlandi\n");
    char *fileContent = readFile("./site.html");
    if (!fileContent)
    {
        printf("HATA: Dosya okunamadı!\n");
        return 1;
    }
    struct response res = *createResponse(200, " ", fileContent);
    free(fileContent);
    printf("res hazırlandı\n");

    struct request req1 = *createRequest("GET","/site.js"," "," ");
    fileContent = readFile("./site.js");
    if (!fileContent)
    {
        printf("HATA: Dosya okunamadı!\n");
        return 1;
    }
    struct response res1= *createResponse(200," ",fileContent);
    appendrequest(&myApp, req, res);
    appendrequest(&myApp,req1,res1);
    printf("İstek sunucuya eklendi\n");

    startServer(&myApp,"127.0.0.1",8080);
    printf("server durdu\n");

    return 0;
}
