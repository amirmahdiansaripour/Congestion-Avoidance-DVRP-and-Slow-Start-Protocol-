#include "receiver.hpp"

using namespace std;

Receiver::Receiver(string port_, int toRouter_, int fromRouter_){
    port = port_;
    toRouter=new Socket(toRouter_);    
    fromRouter=new Socket(fromRouter_);
    Acks = vector<bool>(MAX_PACKET_IND, false);
    content = vector<string>(MAXNUMOFPACKETS);
}

void Receiver::sendAck(string packet){
    indexHeader1 = 0;
    indexHeader2 = 0;
    indexHeader3 = 0;
    indexHeader4 = 0;
    
    for(int i = 0; i < packet.size(); i++){
        if(packet[i] == '/'){
            indexHeader1 = i;
            break;
        }
    }
    for(int j = indexHeader1 + 1; j < packet.size(); j++){
        if(packet[j] == '/'){
            indexHeader2 = j;
            break;
        }
    }

    for(int k = indexHeader2 + 1; k < packet.size(); k++){
        if(packet[k] == '/'){
            indexHeader3 = k;
            break;
        }
    }
    
    for(int t = indexHeader3 + 1; t < packet.size(); t++){
        if(packet[t] == '/'){
            indexHeader4 = t;
            break;
        }
    }
}

int Receiver::searchFirstLost(){
    map<int, bool>:: iterator p;
    for (int i = 0; i < MAX_PACKET_IND; i++){
        if(Acks[i] == false){
            return i;
        }
    }
}

void Receiver::reconstructFile(){
    ofstream res ("result.txt");
    string testString = "";
    for(int i = 0; i < MAXNUMOFPACKETS; i++){
      testString += content[i];
    }
    res << testString;
    res.close();
}

string Receiver::makeAckMessage(int lostPacket){
    string ack = "1/" + to_string(1100 + (rand()%1000)) + "/" + port + "/" + dest + "/" + to_string(lostPacket);
    return ack;
}


bool Receiver::handlePacket(string packet){
    sendAck(packet);
    int currAck = stoi(packet.substr(indexHeader1+1, indexHeader2));
    cout << packet.substr(0, indexHeader2) << "\n";
    Acks[currAck] = true;   
    content[currAck] = packet.substr(indexHeader4 + 1, packet.size());
    int lostPacket = searchFirstLost();
    string message = makeAckMessage(lostPacket);
    cout << "Messsage to router: " << message << "\n";
    toRouter->send(message);
    if(lostPacket == MAXNUMOFPACKETS) return true;
    else return false;
}

void Receiver::run(){
    fd_set currFd, tmpFd;
    FD_ZERO(&currFd);
    int maxFd = max(toRouter->fd, fromRouter->fd);
    FD_SET(STDIN_FILENO, &currFd);
    FD_SET(fromRouter->fd, &currFd);
    FD_SET(toRouter->fd, &currFd);
    int lastAck = 0;
    int cumulativeAck;
    while (true){
        tmpFd = currFd;
        select(maxFd + 1, &tmpFd, NULL, NULL, NULL);
        if (FD_ISSET(fromRouter->fd, &tmpFd)){ // message from router
            string packet = fromRouter->receive(); 
            bool res = handlePacket(packet);
            if(res) break;
        }
        if(FD_ISSET(STDIN_FILENO, &tmpFd)){     // message to router
            string message;
            cin>>message;
            toRouter->send(message);
        }
    }
}