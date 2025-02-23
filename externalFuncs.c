#include "externalFuncs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

void prepareResponse(struct response *src, char **dest) {
    *dest = (char *)malloc(200 * sizeof(char));  // Bellek ayır
    if (*dest == NULL) {
        printf("Bellek tahsisi başarısız!\n");
        return;
    }
    sprintf(*dest, "HTTP/1.1 %d OK \r\nContent-Type: text/html \r\nContent-Length: %d \r\nConnection: close \r\n\r\n%s", 
            src->status, src->contentLenght, src->body);
}

char* readHtmlFile(const char *filename) {
    FILE *file = fopen(filename, "rb");  // Dosyayı okuma modunda aç (binary moduyla)
    if (!file) {
        printf("Dosya açılamadı!\n");
        return NULL;
    }

    // Dosya boyutunu öğren
    fseek(file, 0, SEEK_END);  // Dosya sonuna git
    long fileSize = ftell(file);  // Konumu al (dosyanın toplam uzunluğu)
    rewind(file);  // Dosyanın başına dön

    // Bellek tahsis et (+1 çünkü string için sonuna '\0' ekleyeceğiz)
    char *buffer = (char *)malloc(fileSize + 1);
    if (!buffer) {
        printf("Bellek tahsisi başarısız!\n");
        fclose(file);
        return NULL;
    }

    // Dosyanın tamamını oku
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';  // String'in sonuna '\0' koy

    fclose(file);  // Dosyayı kapat
    return buffer; // Pointer'ı döndür
}