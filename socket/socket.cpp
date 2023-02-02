#include "socket.hpp"

using namespace std;

vector<string> split(string unsplitted, char delimeter){
    string split = "";
    vector<string> lastRes;
    for(int i = 0; i < unsplitted.size(); i++){
        if(unsplitted[i] == delimeter){
            lastRes.push_back(split);
            split = "";
        }
        else
            split.push_back(unsplitted[i]);
    }
    lastRes.push_back(split);
    return lastRes;
}

Socket::Socket(int port) {
    int tcpFd = socket(AF_INET, SOCK_DGRAM, 0);
    int broadcast = 1, opt_en = 1;
    setsockopt(tcpFd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(tcpFd, SOL_SOCKET, SO_REUSEPORT, &opt_en, sizeof(opt_en));
    fd = tcpFd;
    bus.sin_family = AF_INET;
    bus.sin_port = htons(port);
    bus.sin_addr.s_addr = inet_addr("localhost");
    bind(tcpFd, (struct sockaddr *)&bus, sizeof(bus));
}

int Socket::send(string message){
    return sendto(fd, message.c_str(), message.size(), 0,(struct sockaddr *)&bus, sizeof(bus));
}

std::string Socket::receive(){
    char buff[1049] = {0};
    recv(fd, buff, 1024, 0);
    return buff;
}

int Socket::getFd(){return fd;}