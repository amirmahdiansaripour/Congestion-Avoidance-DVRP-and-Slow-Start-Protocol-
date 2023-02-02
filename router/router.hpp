#ifndef __ROUTER_H__
#define __ROUTER_H__

#include"../socket/socket.hpp"
#include <map>

const int QUEUESIZE = 10;   // infinity (10000) is the first case
const int MAXNUMOFPACKETS = 1100;
const int DELAYCOEF = 100;
const int DROPPROBABILITY = 10; // 0 if no need to drop

class Router{
private:
    std::string port;
    Socket* toSender;
    Socket* fromSender;
    Socket* toReceiver;
    Socket* fromReceiver;
    void findHeader(std::string);
    void decideOnDropped();
public:
    Router(std::string, int, int, int, int);
    std::map<int, bool> droppedPackets;
    void run();
    std::vector<int> dropped;
    std::vector<std::string> queue;
    clock_t lastPacketSent;
    void showQueueContent();
    int numOfSents;
    int indexHeader1, indexHeader2;
};


#endif