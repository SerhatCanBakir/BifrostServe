#ifndef APP_H
#define APP_H

struct request
{
    char *method;
    char *url;
    char *headers;
    char *body;
};

struct response
{
    int status;
    char *contentType;
    unsigned int contentLenght;
    char *body;
};

typedef struct 
{
    unsigned int reqSize;
    unsigned int resSize; 
    struct request req[50];
    struct response res[50];
} APP;

int appInit(APP* app);
int copyUntilSpace(const char *src, char *dest);
void prepareResponse(struct response *src, char **dest);
char* readFile(const char *filename);
int checkReq(APP* app,char* comingRequest,char** returnResp);
int appendrequest(APP* app,struct request req,struct response res);
void appDestroy(APP* app);
int startServer(APP *app,char* ipAddr,int PORT);
struct request* createRequest(char* method,char*url,char* header,char* body);
struct response* createResponse(int status,char* contentType,char* body);
#endif