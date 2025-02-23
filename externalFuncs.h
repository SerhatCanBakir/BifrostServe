#ifndef EXTERNALFUNCS_H
#define EXTERNALFUNCS_H

struct req{
    char* method;
    char* url;
    char* headers;
    char* body;
    };

struct response
{
    int status;
    char* contentType;
    unsigned int contentLenght;
    char* body; 
};

char* readHtmlFile(const char* src);

    int copyUntilSpace(const char *src, char *dest);
    void prepareResponse(struct response* src,char** dest);
#endif
