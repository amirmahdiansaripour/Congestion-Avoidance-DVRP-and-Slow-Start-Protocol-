#include "receiver.hpp"

using namespace std;

void Receiver::setSockets(){
    if(port == "172.16.0.10"){
        toRouter = new Socket(8001);
        fromRouter = new Socket(8002);
    }
    else if(port == "172.16.0.20"){
        toRouter = new Socket(8003);
        fromRouter = new Socket(8004);
    }
    else if(port == "172.16.29.1"){
        toRouter = new Socket(8029);
        fromRouter = new Socket(8030);
    }
    else if(port == "172.16.2.1"){
        toRouter = new Socket(8032);
        fromRouter = new Socket(8033);
    }
}

void Receiver::extractRoutingTable(){
    string path = "../routingtables/" + port + ".txt";
    ifstream file(path);
    string row;
    while(getline(file, row)){
        vector<string> half_split = split(row, '\t');
        string first = split(half_split[0], ' ')[1];
        string second = split(half_split[2], ' ')[1];
        // cout << "first " << first << "\tsecond " << second << "\n";
        routingTable[first] = second;
    }
}


Receiver::Receiver(string port_){
    port = port_;
    extractRoutingTable();
    setSockets();
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
    string ack = "1/" + to_string(1100 + (rand()%1000)) + "/" + port + "/" + dest_port + "/" + to_string(lostPacket);
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
    // cout << "Messsage to router: " << message << "\n";
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
            if(dest_port == ""){
                sendAck(packet);
                int ind;
                for(ind = indexHeader2 + 1; ind < indexHeader3; ind++) dest_port += packet[ind]; 
            }
            bool res = handlePacket(packet);
            if(res) break;
        }
        if(FD_ISSET(STDIN_FILENO, &tmpFd)){     // message to router
            string message;
            cin>>message;
            cout << "MM\n";
            toRouter->send(message);
        }
    }
}