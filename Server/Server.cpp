#include "Server.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

static void sighandler(int sig) 
{
    flagShutdown = 1;
}

void* cryptoFunc(void *arg) 
{
    argFunc *argCrypto = (argFunc*)arg;
    std::string resCrypto, hashString, encoded;
    resCrypto = "cryptoFunc: ";

    CryptoPP::SHA1 hash;
    hash.Update((const CryptoPP::byte*)(argCrypto->buf + 7), 32);
    hashString.resize(hash.DigestSize());
    hash.Final((CryptoPP::byte*)&hashString[0]);

    CryptoPP::HexEncoder encoder;
    encoder.Put((CryptoPP::byte*)&hashString[0], hashString.size());
    encoder.MessageEnd();
    int sizeEncoded = encoder.MaxRetrievable();
    std::cout << hashString.size() << std::endl;
    if (sizeEncoded) 
    {
        encoder.Get((CryptoPP::byte*)&encoded[0], sizeEncoded);
        std::cout << encoded << std::endl;
    }

    resCrypto += "before encode - ";
    resCrypto += argCrypto->buf;
    resCrypto += " after encode - ";
    resCrypto += encoded;

    if (send(argCrypto->fd, resCrypto.c_str(), resCrypto.size(), 0) == -1) 
    {
        perror("send");
        exit(EXIT_FAILURE);
    } 
}

void* generalFunc(void *arg) 
{
    argFunc *argGeneral = (argFunc*)arg;
    std::string resGeneral;
    resGeneral = "generalFunc: ";
    resGeneral += argGeneral->buf;
    if (send(argGeneral->fd, resGeneral.c_str(), resGeneral.size(), 0) == -1) 
    {
        perror("send");
        exit(EXIT_FAILURE);
    } 
}

void Server::Initialize() 
{
    std::cout << "Server: Initialize" << std::endl;

    int flagSetSockOpt = 1;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int resGetAddrInfo = getaddrinfo(NULL, PORT, &hints, &res);
    if (resGetAddrInfo) 
    {
        std::cout << "getaddrinfo: " << gai_strerror(resGetAddrInfo) << std::endl;
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next) 
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flagSetSockOpt, sizeof(int)) == -1) 
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("bind");
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    if (p == NULL) 
    {
        std::cout << "failed to bind" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) == -1) 
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Set sighandler" << std::endl;

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL)) 
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    threadPool.Initialize();
}

void Server::Start() 
{
    int fdClient;
    char buf[BUFSIZE];

    while (!flagShutdown) 
    {
        if ((fdClient = accept(sockfd, NULL, NULL)) == -1) 
        {
            if (flagShutdown) 
            {
                break;
            }
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (recv(fdClient, buf, BUFSIZE, 0) == -1) 
        {
            if (flagShutdown) 
            {
                break;
            }
            perror("recv");
            exit(EXIT_FAILURE);
        }

        if (strstr(buf, "crypto")) 
        {
            threadPool.AddTask(TaskWrapper(cryptoFunc, std::make_shared<argFunc>(fdClient, buf).get()));
        }
        else 
        {
            threadPool.AddTask(TaskWrapper(generalFunc, std::make_shared<argFunc>(fdClient, buf).get()));
        }

        threadPool.PopTask();
    }

    Shutdown();
}

void Server::Shutdown() 
{
    std::cout << "Server: Shutdown" << std::endl;
    threadPool.Shutdown();
    close(sockfd);
    std::cout << "Shutdown finished" << std::endl;
}