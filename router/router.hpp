#ifndef __ROUTER_H__
#define __ROUTER_H__

#include "../network/logger.hpp"
#include"../socket/socket.hpp"
#include <map>

const int QUEUESIZE = 5;   // infinity (10000) is the first case
const int MAXNUMOFPACKETS = 1100;
const int DELAYCOEF = 100;
const int DROPPROBABILITY = 0; // 0 if no need to drop

class Router{
private:
    std::string port;
    std::map<std::string, std::string> routingTable;
    std::map<std::string, Socket*> tos;
    std::vector<Socket*> froms;
    Socket *fromSender1, *fromReceiver1, *toReceiver1, *toSender1, *fromSender2, *fromReceiver2, *toReceiver2, *toSender2;
    std::map<int, bool> droppedPackets;
    std::vector<int> dropped;
    int firstTransmit;
    std::vector<std::string> queue;
    clock_t lastPacketSent;
    int numOfSents;
    Logger logger;
    int indexHeader1, indexHeader2, indexHeader3, indexHeader4;
public:
    Router(std::string);
    void run();
    void handleTimeout(Socket*);
    void handleDirectLines();
    void handleSingleSockets();
    void handleTransmit();
    void handleMultiConnections();
    // void handleNextPackets();
    void setSockets();
    void showQueueContent();
    void findHeader(std::string);
    void decideOnDropped();
    void extractRoutingTable();
};


#endif