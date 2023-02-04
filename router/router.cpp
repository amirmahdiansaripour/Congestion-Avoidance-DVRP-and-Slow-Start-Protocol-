#include "router.hpp"

using namespace std;

void Router::setSockets(){
    if(port == "172.16.0.0"){ // blue
        fromSender1 = new Socket(8013);
        toSender1 = new Socket(8014);
        toReceiver1 = new Socket(8002);
        fromReceiver1 = new Socket(8001);
    }
    if(port == "172.16.29.0"){ // red
        fromSender1 = new Socket(8025);
        toSender1 = new Socket(8026);
        fromReceiver1 = new Socket(8029);
        toReceiver1 = new Socket(8030);
    }
    if(port == "172.16.1.0"){
        fromSender1 = new Socket(8007); // red
        toSender1 = new Socket(8008); // red
        toReceiver1 = new Socket(8017); // red
        fromReceiver1 = new Socket(8018); // red
        fromSender2 = new Socket(8023);
        toSender2 = new Socket(8024);
        toReceiver2 = new Socket(8013); //172.16.0.0
        fromReceiver2 = new Socket(8014);
    }
    if(port == "172.16.2.0"){
        fromSender1 = new Socket(8017); //red
        toSender1 = new Socket(8018); // red
        toReceiver1 = new Socket(8027); //red
        fromReceiver1 = new Socket(8028); //red
        // fromSender2 = new Socket(8028); //black
        // toSender2 = new Socket(8027); // black
        fromReceiver2 = new Socket(8032); //black
        toReceiver2 = new Socket(8033);   //black
    }
    if(port == "172.16.10.0"){
        fromSender1 = new Socket(8015); // black
        toSender1 = new Socket(8016); // black
        toReceiver1 = new Socket(8028); // black
        fromReceiver1 = new Socket(8027);  // black
        // fromSender2 = new Socket(8027); // red
        // toSender2 = new Socket(8028); // red
        toReceiver2 = new Socket(8025); // red
        fromReceiver2 = new Socket(8026); // red
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
    // extractRoutingTable();
    setSockets();
    decideOnDropped();
    firstTransmit = 0;
}

void Router::decideOnDropped(){
    vector<int> dropindex;
    if(DROPPROBABILITY > 0){
        srand(time(0));
        for(int i = 0; i < MAXNUMOFPACKETS; i+= DROPPROBABILITY)
            dropindex.push_back(i + (rand() % DROPPROBABILITY));
        
        // cout << "drop indexes : ";
        for(int j : dropindex){
            // cout << j << " ";
            droppedPackets[j] = true;
        }    
    }
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
        string ss = "";
        int ind = 0;
        for(int i = 0; i < s.size(); i++){
            if(s[i] == '/'){
                if(ind == 1) break;
                ind++;
            }
            ss += s[i];
        }
        cout << ss << "\n";
    }
}

int max(int a, int b, int c, int d, int e = 0, int f = 0, int g = 0, int h = 0){
    int res = a;
    if(res < b) res = b;
    if(res < c) res = c;
    if(res < d) res = d;
    if(res < e) res = e;
    if(res < f) res = f;
    if(res < g) res = g;
    if(res < h) res = h;
    return res;  
}

void Router::handleTimeout(Socket* soc){
    if(queue.empty() == false){
        findHeader(queue[0]);
        if(indexHeader1 + 1 < indexHeader2){
            int id = stoi(queue[0].substr(indexHeader1 + 1, indexHeader2));
            if(droppedPackets[id] == false){    
                soc->send(queue[0]);    
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

void Router::handleSingleSockets(){
    fd_set currFd, tempFd;
    FD_ZERO(&currFd);
    int maxFd = max(fromSender1->fd, toSender1->fd, toReceiver1->fd, fromReceiver1->fd);
    FD_SET(fromSender1->fd, &currFd);
    FD_SET(toSender1->fd, &currFd);
    FD_SET(toReceiver1->fd, &currFd);
    FD_SET(fromReceiver1->fd, &currFd);
    // cout << "HI\n";
    while (true){
        tempFd = currFd;
        select(maxFd + 1, &tempFd, NULL, NULL, NULL);
        if(FD_ISSET(fromSender1->fd, &tempFd)){
            string packet = fromSender1->receive();
            if(queue.size() < QUEUESIZE){
                queue.push_back(packet);
            }
        }
        if (FD_ISSET(fromReceiver1->fd, &tempFd)){
            // cout << "HIHI\n";
            string ackMessage = fromReceiver1->receive();   
            // cout << ackMessage << "\n";
            toSender1->send(ackMessage);
        }
        if((clock()-lastPacketSent)/(CLOCKS_PER_SEC/DELAYCOEF)>1){
            lastPacketSent = clock();
            handleTimeout(toReceiver1);
        }
    }
}

void Router::handleDirectLines(){
    fd_set currFd, tempFd;
    FD_ZERO(&currFd);
    int maxFd = max(fromSender1->fd, toSender1->fd, toReceiver1->fd, fromReceiver1->fd, fromSender2->fd, toSender2->fd, toReceiver2->fd, fromReceiver2->fd);
    FD_SET(fromSender1->fd, &currFd);
    FD_SET(toSender1->fd, &currFd);
    FD_SET(toReceiver1->fd, &currFd);
    FD_SET(fromReceiver1->fd, &currFd);
    FD_SET(fromSender2->fd, &currFd);
    FD_SET(toSender2->fd, &currFd);
    FD_SET(toReceiver2->fd, &currFd);
    FD_SET(fromReceiver2->fd, &currFd);
    while (true){
        tempFd = currFd;
        select(maxFd + 1, &tempFd, NULL, NULL, NULL);
        if(FD_ISSET(fromSender1->fd, &tempFd)){
            string packet = fromSender1->receive();
            if(queue.size() < QUEUESIZE){
                queue.push_back(packet);
            }
        }
        if(FD_ISSET(fromSender2->fd, &tempFd)){
            string packet = fromSender2->receive();
            if(queue.size() < QUEUESIZE){
                queue.push_back(packet);
            }
        }
        if(FD_ISSET(fromReceiver1->fd, &tempFd)){
            string ackMessage = fromReceiver1->receive();   
            toSender1->send(ackMessage);
        }
        if (FD_ISSET(fromReceiver2->fd, &tempFd)){
            string ackMessage = fromReceiver2->receive();  
            // cout << ackMessage << "\n"; 
            toSender2->send(ackMessage);
        }
        if((clock()-lastPacketSent)/(CLOCKS_PER_SEC/DELAYCOEF)>1){
            lastPacketSent = clock();
            if(queue.size() > 0){
                findHeader(queue[0]);
                string desti = "";
                // cout << indexHeader3 << "\t" << indexHeader4 << "\n";
                for(int l = indexHeader3 + 1; l < indexHeader4; l++){
                    desti += queue[0][l];
                }
                // cout << "desti: " << desti << "\n";
                if(desti == "172.16.0.10")    handleTimeout(toReceiver2);
                else    handleTimeout(toReceiver1);
            }
        }
    }
}

void Router::handleMultiConnections(){
    fd_set currFd, tempFd;
    FD_ZERO(&currFd);
    int maxFd = max(fromSender1->fd, toSender1->fd, toReceiver1->fd, fromReceiver1->fd, toReceiver2->fd, fromReceiver2->fd);
    FD_SET(fromSender1->fd, &currFd);
    FD_SET(toSender1->fd, &currFd);
    FD_SET(toReceiver1->fd, &currFd);
    FD_SET(fromReceiver1->fd, &currFd);
    FD_SET(toReceiver2->fd, &currFd);
    FD_SET(fromReceiver2->fd, &currFd);
    while (true){
        tempFd = currFd;
        select(maxFd + 1, &tempFd, NULL, NULL, NULL);
        if(FD_ISSET(fromSender1->fd, &tempFd)){
            string packet = fromSender1->receive();
            if(queue.size() < QUEUESIZE){
                queue.push_back(packet);
            }
        }
        if(FD_ISSET(fromReceiver1->fd, &tempFd)){   // equals fromSender2
            string packet = fromReceiver1->receive();
            if(packet[0] == '0'){   //PACKET
                if(queue.size() < QUEUESIZE){
                    queue.push_back(packet);
                }
            }
            else{   //ACK
                toSender1->send(packet);
            }
        }
        else if(FD_ISSET(fromReceiver2->fd, &tempFd)){
            string ackMessage = fromReceiver2->receive();   
            toReceiver1->send(ackMessage);
        }
        if((clock()-lastPacketSent)/(CLOCKS_PER_SEC/DELAYCOEF)>1){
            lastPacketSent = clock();
            if(queue.size() > 0){
                findHeader(queue[0]);
                string desti = "";
                for(int l = indexHeader3 + 1; l < indexHeader4; l++){
                    desti += queue[0][l];
                }
                if(desti == "172.16.2.1"){
                    if(port == "172.16.2.0") handleTimeout(toReceiver2);
                    else if(port == "172.16.10.0") handleTimeout(toReceiver1);
                }    
                else if(desti == "172.16.29.1"){
                    if(port == "172.16.2.0") handleTimeout(toReceiver1);
                    else if(port == "172.16.10.0") handleTimeout(toReceiver2);
                }
            }
        }
    }
}

void Router::handleTransmit(){
    if(port == "172.16.29.0" || port == "172.16.0.0"){
        handleSingleSockets();
    }
    else if (port == "172.16.1.0"){
        handleDirectLines();
    }

    else if (port == "172.16.2.0" || port == "172.16.10.0"){
        handleMultiConnections();
    }
}

void Router::run() {
    handleTransmit();
}  