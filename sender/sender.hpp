#ifndef __SENDER_H__
#define __SENDER_H__

#include <math.h>
#include <map>
#include <fstream>
#include "../network/logger.hpp"
#include "../socket/socket.hpp"


// const std::string dest = "172.16.0.1";
const int MAXNUMOFPACKETS = 1100;
const int INF = 10000;
const int NEWLINE = 127;
const int DELAYCOEF = 1;    // increase leads to long timeouts

class Sender{
public:
    Sender(std::string, std::string);
    void run();
    int extractAck(std::string);
    void sendPacketsBasedCwd();
    void updateCWND();
    void handleTimeout();
    void findHeader(std::string);
    void recordData();
    void splitIntoPackets();
    void makePackets();
    void extractRoutingTable();
    void setSockets();
    Logger logger;
private:
    clock_t start;
    clock_t end;
    std::vector<int> cwnds;
    std::vector<int> threasholds;
    std::vector<std::string> packets;
    std::vector<std::string> content;
    std::map<std::string, std::string> routingTable;
    Socket* toRouter;
    Socket* fromRouter;
    int lastPacketSent;
    int ack;
    int cwnd;
    std::string port, dest_port;
    clock_t lastDepart;
    bool firstRound;
    int treshold;
    int indexHeader1, indexHeader2, indexHeader3, indexHeader4;
};

#endif