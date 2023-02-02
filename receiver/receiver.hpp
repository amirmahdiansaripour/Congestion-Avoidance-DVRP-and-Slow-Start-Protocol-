#ifndef __RECEIVER_H__
#define __RECEIVER_H__

#include "../socket/socket.hpp"
#include <map>
#include <fstream>
#include <string>

const int MAX_PACKET_IND = 1110;
const int MAXNUMOFPACKETS = 1100;
const std::string dest = "127.0.0.1";

class Receiver{
public:
    Receiver(std::string, int, int);
    void run();
    int searchFirstLost();
    void sendAck(std::string);
    bool handlePacket(std::string);
    void reconstructFile();
    std::string makeAckMessage(int);
private:
    std::vector<bool>Acks;
    Socket* toRouter;
    Socket* fromRouter;
    std::string port;
    std::vector<std::string> content;
    int indexHeader1, indexHeader2, indexHeader3, indexHeader4;
};


#endif