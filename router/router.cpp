#include "router.hpp"

using namespace std;

void Router::setSockets(){
    if(port == "172.16.0.0"){
        froms = vector<Socket*>(5);
        froms[0] = new Socket(8001);
        froms[1] = new Socket(8003);
        froms[2] = new Socket(8005);
        froms[3] = new Socket(8009);
        froms[4] = new Socket(8013);
        tos["172.16.0.10"] = new Socket(8002);
        tos["172.16.0.20"] = new Socket(8004);
        tos["172.16.10.0"] = new Socket(8010);
        tos["172.16.1.0"] = new Socket(8014);
        tos["172.16.0.1"] = new Socket(8006);
    }
}


void Router::extractRoutingTable(){
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


Router::Router(string port_) {
    port = port_;
    lastPacketSent = clock();
    extractRoutingTable();
    setSockets();
    decideOnDropped();
    firstTransmit = 0;
}

void Router::decideOnDropped(){
    vector<int> dropindex;
    if(DROPPROBABILITY > 0){
        for(int i = 0; i < MAXNUMOFPACKETS; i+= DROPPROBABILITY)
            dropindex.push_back(i + (rand() % DROPPROBABILITY));
        
        // cout << "drop indexes : ";
        for(int j : dropindex){
            // cout << j << " ";
            droppedPackets[j] = true;
        }    
    }
}

int Router::max(){
    int res = froms[0]->fd;
    for(int i = 1; i < froms.size(); i++)
        if(froms[i]->fd > res) res = froms[i]->fd;
    
    map<string, Socket*> :: iterator it;
    for(it = tos.begin(); it != tos.end(); it++)
        if(it->second->fd > res) res = it->second->fd;
    
    return res;
}

void Router::findHeader(string packet){
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

void Router::showQueueContent(){
    cout << "Queue content: \n";
    for(string s : queue){
        findHeader(s);
        cout << s.substr(0, indexHeader2) << "\n";
    }
}

int maxa(int a, int b, int c, int d){
    int res = a;
    if(res < b) res = b;
    if(res < c) res = c;
    if(res < d) res = d;
    return res;  
}

void Router::handleNextPackets(){
    fd_set currFd2, tempFd2;
    FD_ZERO(&currFd2);
    FD_SET(fromReceiver->fd, &currFd2);
    FD_SET(fromSender->fd, &currFd2); 
    FD_SET(toSender->fd, &currFd2);
    FD_SET(toReceiver->fd, &currFd2); 
    int maxFd = maxa(toReceiver->fd, toSender->fd, fromReceiver->fd, fromSender->fd);
    while(true){
        tempFd2 = currFd2;
        select(maxFd + 1, &tempFd2, NULL, NULL, NULL);
        if (FD_ISSET(fromSender->fd, &tempFd2)){
            string packet = fromSender->receive();
            if(queue.size() < QUEUESIZE){
                queue.push_back(packet);
            }
            cout << packet.substr(0, 10);
        }
        else if (FD_ISSET(fromReceiver->fd, &tempFd2)){
            string ackMessage = fromReceiver->receive();   
            toSender->send(ackMessage);
        }
        if((clock()-lastPacketSent)/(CLOCKS_PER_SEC/DELAYCOEF)>1){
            lastPacketSent = clock();
            if(queue.empty() == false){
                findHeader(queue[0]);
                if(indexHeader1 + 1 < indexHeader2){
                    int id = stoi(queue[0].substr(indexHeader1 + 1, indexHeader2));
                    if(droppedPackets[id] == false){    
                        toReceiver->send(queue[0]);
                        numOfSents++;
                    }
                    else{
                        cout << "PACKET IS DROPPED\n";
                        droppedPackets[id] = false;     // dont drop this anymore
                    }
                    showQueueContent();
                    queue.erase(queue.begin());
                }
            }
        }
    }
}

void Router::handleTransmit(fd_set tempFd){
    for(int i = 0; i < froms.size(); i++){
        if(FD_ISSET(froms[i]->fd, &tempFd)){
            string packet = froms[i]->receive();
            findHeader(packet);
            string desti = "";
            for(int l = indexHeader3 + 1; l < indexHeader4; l++){
                desti += packet[l];
            }
            if(packet[0] == '1'){   // First ACK
                fromReceiver = froms[i];
                toSender = tos[desti];
                tos[desti]->send(packet);
            }
            else{   // First packet
                fromSender = froms[i];
                toReceiver = tos[desti];
                toReceiver->send(packet);
                handleNextPackets();
            }
        }
    }
}

void Router::run() {
    fd_set currFd, tempFd;
    FD_ZERO(&currFd);
    int maxFd = max();
    for(int i = 0; i < froms.size(); i++)
        FD_SET(froms[i]->fd, &currFd);
    int counter = 0;
    while (true){
        tempFd = currFd;
        select(maxFd + 1, &tempFd, NULL, NULL, NULL);
        handleTransmit(tempFd);
    }
}  