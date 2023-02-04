#ifndef __RECEIVER_H__
#define __RECEIVER_H__

#include "../network/logger.hpp"
#include "../socket/socket.hpp"
#include <map>
#include <fstream>
#include <string>

// const std::string dest = "127.0.0.1";

class Receiver{
public:
    Receiver(std::string);
    void run();
    int searchFirstLost();
    void sendAck(std::string);
    void setSockets();
    bool handlePacket(std::string);
    void reconstructFile();
    void extractRoutingTable();
    std::string makeAckMessage(int);
    void rerun();
private:
    std::vector<bool>Acks;
    Socket* toRouter;
    Logger logger;
    Socket* fromRouter;
    std::map<std::string, std::string> routingTable;
    std::string port, dest_port;
    std::vector<std::string> content;
    int numberOfRuns;
    int indexHeader1, indexHeader2, indexHeader3, indexHeader4;
    int MAX_PACKET_IND, MAXNUMOFPACKETS;
};


#endif