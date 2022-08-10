#ifndef _SERVER_H_
#define _SERVER_H_

#include "../ThreadPool/ThreadPool.h"
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT "8800"
#define BACKLOG 15
#define BUFSIZE 255

struct argFunc 
{
    int fd;
    char *buf;

    argFunc(int _fd, char *_buf) : fd(_fd), buf(_buf) {}
};

class Server 
{
    private:
        int sockfd;
        ThreadPool threadPool;

    public:
        void Initialize();
        void Start();
        void Shutdown();
};

#endif