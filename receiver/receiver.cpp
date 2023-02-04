#include "receiver.hpp"

using namespace std;

void Receiver::setSockets(){
    if(port == "172.16.0.10"){
        toRouter = new Socket(8001);
        fromRouter = new Socket(8002);
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

void Receiver::rerun(){
    Acks = vector<bool>(MAX_PACKET_IND, false);
    content = vector<string>(MAXNUMOFPACKETS);
    run();
}

Receiver::Receiver(string port_){
    MAX_PACKET_IND = 1110;
    MAXNUMOFPACKETS = 1100;
    port = port_;
    if(port == "172.16.2.1"){
        MAX_PACKET_IND = 3330;
        MAXNUMOFPACKETS = 3300;
    }
    numberOfRuns = 0;
    extractRoutingTable();
    setSockets();
    rerun();
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
    string path = "result" + port;
    if(port == "172.16.29.1"){
        path += to_string(numberOfRuns);
        numberOfRuns++;
    }
    path += ".txt";
    ofstream res (path);
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
    logger.add("ACK IS SENT ON PORT " + to_string(toRouter->pp));
    if(lostPacket == MAXNUMOFPACKETS){
        if(port == "172.16.29.1"){  // again
            reconstructFile();
            Acks = vector<bool>(MAX_PACKET_IND, false);
            content = vector<string>(MAXNUMOFPACKETS);
            toRouter->send("1/1010/" + port + "/" + dest_port + "/0");
            return false;
        }
        else{
            toRouter->send(message);
            return true;
        } 
        
    }
    else{
        toRouter->send(message);    
        return false;
    }
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
    toRouter->send("1/1010/" + port + "/" + dest_port + "/0");
    while (true){
        tmpFd = currFd;
        select(maxFd + 1, &tmpFd, NULL, NULL, NULL);
        if (FD_ISSET(fromRouter->fd, &tmpFd)){ // message from router
            string packet = fromRouter->receive(); 
            logger.add("PACKET IS RECEIVED FROM ROUTER ON PORT " + to_string(fromRouter->pp));
            if(dest_port == ""){
                sendAck(packet);
                int ind;
                for(ind = indexHeader2 + 1; ind < indexHeader3; ind++) dest_port += packet[ind]; 
            }
            bool res = handlePacket(packet);
            if(res){
                logger.finalWrite();
                break;
            }
        }
        if(FD_ISSET(STDIN_FILENO, &tmpFd)){     // message to router
            string message;
            cin>>message;
            toRouter->send(message);
            logger.add("ACK IS SENT ON PORT " + to_string(toRouter->pp));
        }
    }
}