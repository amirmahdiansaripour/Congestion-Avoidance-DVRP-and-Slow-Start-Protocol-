#include "sender.hpp"

using namespace std;

void Sender::setSockets(){
    if(port == "172.16.0.10"){
        toRouter = new Socket(8001);
        fromRouter = new Socket(8002);
    }
    else if(port == "172.16.0.20"){
        toRouter = new Socket(8003);
        fromRouter = new Socket(8004);
    }
    else if(port == "172.16.0.1"){
        if(routingTable[dest_port] == "172.16.0.0"){
            toRouter = new Socket(8005);
            fromRouter = new Socket(8006);
        }
        else if(routingTable[dest_port] == "172.16.1.0"){
            toRouter = new Socket(8007);
            fromRouter = new Socket(8008);
        }
    }
}


void Sender::extractRoutingTable(){
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

Sender::Sender(string port_, string dest_port_){
    port = port_;
    dest_port = dest_port_;
    extractRoutingTable();
    setSockets();
    lastPacketSent = 0;
    cwnd = 1;
    firstRound = true;
    makePackets();
    start = clock();
    treshold = INF;
    threasholds.push_back(1e9);
}

void Sender::recordData(){
    ofstream file("reports.txt");
    end = clock();
    int duration = (end - start)/(CLOCKS_PER_SEC);
    file << "Duration: " << duration << " s\n";
    int maxCwnd = cwnds[0];
    file << "Cwnd values: \n";
    for(int i = 0; i < cwnds.size(); i++){
        file << cwnds[i] << "\n";
        if(cwnds[i] > maxCwnd) maxCwnd = cwnds[i];
    }
    file << "Threasholds: \n";
    for(int j = 0; j < threasholds.size(); j++){
        file << threasholds[j] << "\n";
    }
    file << "Max cwnd: " << maxCwnd << "\n";
    file.close();
}

int Sender::extractAck(string message){
    findHeader(message);
    return stoi(message.substr(indexHeader4 + 1, message.size()));
}

void Sender::findHeader(string packet){
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


void Sender::sendPacketsBasedCwd(){
    for(int i = lastPacketSent; i < lastPacketSent + cwnd; i++){
        if(i < packets.size()){
            findHeader(packets[i]);
            // cout << packets[i] << "\n";
            // cout << "index1 " << indexHeader1 << " index2 " << indexHeader2 << "\n";
            cout << "sent to Router: " << packets[i].substr(0, indexHeader2) << "\n";
            toRouter->send(packets[i]);
        }
    }
    int first = lastPacketSent;
    lastPacketSent += (cwnd - 1);
    int second = lastPacketSent;
    cout << "cwndSize: " << (second - first + 1) << "\n";
}

void Sender::run() {
    fd_set currFd, tempFd;
    int maxFd = max(fromRouter->fd, toRouter->fd);
    FD_ZERO(&currFd);
    FD_SET(STDIN_FILENO, &currFd);
    FD_SET(fromRouter->fd, &currFd);
    FD_SET(toRouter->fd, &currFd);
    
    while (true){
        tempFd = currFd;
        select(maxFd + 1, &tempFd, NULL, NULL, NULL);
        if (FD_ISSET(fromRouter->fd, &tempFd)){ // getfromrouter
           string message = fromRouter->receive();
           cout << "Message from router: " << message << "\n";
           if(firstRound){
                firstRound = false;
                toRouter->send(packets[0]);
                lastPacketSent = 0;
                lastDepart = clock();
           }
           else{
                ack = extractAck(message);
                cout<<"ACK" << ack<<endl;       
                if(ack == packets.size()){cout << "FINISH";break;}
                if(ack >= lastPacketSent + 1){
                    updateCWND();
                }
           }
        }
        if((float)(clock()-lastDepart)/(CLOCKS_PER_SEC * DELAYCOEF) > 1.0){
            handleTimeout();
        }
    }
}  

void Sender::handleTimeout(){
    lastDepart = clock();
    cout << "Timeout\n";
    treshold = cwnd / 2;    // multiple decrease
    threasholds.push_back(treshold);
    cwnd = 1;
    cwnds.push_back(cwnd);
    lastPacketSent = ack;
    sendPacketsBasedCwd();
}

void Sender::updateCWND(){
        lastPacketSent = ack;
        lastDepart = clock();
        if(cwnd >= treshold){
            cwnd++; // additive increase
            cout << "ADDITIVE INCREASE\n";
            cwnds.push_back(cwnd);
        }
        else{
            cwnd *= 2; // slowstart
            cwnds.push_back(cwnd);
        }
        sendPacketsBasedCwd();
}


void Sender::makePackets(){
    splitIntoPackets();
    packets = vector<string>(MAXNUMOFPACKETS);
    for(int i = 0; i < MAXNUMOFPACKETS; i++){
        packets[i] = ("0/" + to_string(i) + "/" + port + "/" + dest_port + "/" + content[i]);
        // packets[i] += content[i];
    }
}

void Sender::splitIntoPackets(){
    ifstream read("t.txt");
    // ofstream writee("f.txt");
    string largeString;
    string additive;
    while (getline (read, additive)) {
      largeString += additive;
    }
    // cout << "SIZE: " << largeString.size() << "\n";
    read.close();
    int index = 0;
    int offset = (int) (largeString.size() / MAXNUMOFPACKETS);
    int newLineOffset = 0;
    for(int i = 0; i < MAXNUMOFPACKETS; i++){
      string t = "";
      int limit = (i == MAXNUMOFPACKETS - 1) ? largeString.size() : (index + offset);
      for(int j = index; j < limit; j++){
        t += largeString[j];
        newLineOffset++;
        if(newLineOffset == NEWLINE){
          newLineOffset = 0;
          t += '\n';
        }
      }
      index += offset;
      content.push_back(t);
    }
}
