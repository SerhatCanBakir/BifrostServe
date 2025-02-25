#include "app.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void *addRand(int argSize, void **args);
void *postRequsetHandle(int argSize, void **args);

int main()
{
    srand(time(NULL));
    APP myApp;
    appInit(&myApp);
    printf("app init calisti\n");

    // Dinamik olarak request ve response structlarını oluşturulması gerek
    struct request req = *createRequest("GET", "/");
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

    struct request req1 = *createRequest("GET", "/site.js");
    fileContent = readFile("./site.js");
    if (!fileContent)
    {
        printf("HATA: Dosya okunamadı!\n");
        return 1;
    }
    struct response res1 = *createResponse(200, " ", fileContent);
    // dinamic response yazılır
    struct request req3 = *createRequest("GET", "/randomnumber");
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 30;
    *b = 31;
    void *args[2] = {a, b};
    struct response res3 = *createResponseDynamic(200, "text/html", "\"randomNumber : %s\" ", addRand, 2, args);
    // Post isteği örneği
    struct request req4 = *createRequest("POST", "/postnumber");
    void *args1[1];
    struct response res4 = *createResponseDynamic(200, "text/html", "gelen data %s             ", postRequsetHandle, 0, args1);
    // app içine request ve responselar eklenir
    appendrequest(&myApp, req, res);
    appendrequest(&myApp, req1, res1);
    appendrequest(&myApp, req3, res3);
    appendrequest(&myApp, req4, res4);
    printf("İstek sunucuya eklendi\n");

    // app dinlenmeye başlanır
    startServer(&myApp, "127.0.0.1", 8080);
    printf("server durdu\n");
    free(a);
    free(b);
    // bellek sızıntısı olmasın diye bellek serbest bırakılır
    appDestroy(&myApp);
    return 0;
}

void *addRand(int argSize, void **args)
{
    if (argSize < 2 || !args[0] || !args[1])
    {
        printf("HATA: Geçersiz argümanlar\n");
        return NULL;
    }

    int a = *(int *)args[0];
    int b = *(int *)args[1];
    int sum = a + b;

    // Bellek tahsisi (32 bayt yeterli)
    char *temp = malloc(32);
    if (!temp)
        return NULL;

    sprintf(temp, "%d", sum);

    // **Rastgele sayı atama işlemi**
    *(int *)args[0] = rand() % 100; // 0-99 arasında rastgele sayı
    *(int *)args[1] = rand() % 100; // 0-99 arasında rastgele sayı

    return (void *)temp;
}

void *postRequsetHandle(int argSize, void **args)
{
    return (void *)args[0];
}