#ifndef __SOCKET_H__
#define __SOCKET_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <iostream>

#define STDIN 0

class Socket{
public:
    Socket(int);
    void setup();
    int send(std::string);
    std::string receive();
    int getFd();
    int fd;
private:
    int socketPort;
    struct sockaddr_in bus;
    char buffer[255];
};





#endif