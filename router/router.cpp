#include "router.hpp"

using namespace std;

Router::Router(string port_, int toSender_, int fromSender_, int toReceiver_, int fromReceiver_) {
    port = port_;
    toSender=new Socket(toSender_);
    fromSender=new Socket(fromSender_);
    toReceiver=new Socket(toReceiver_);
    fromReceiver=new Socket(fromReceiver_);
    lastPacketSent = clock();
    decideOnDropped();
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

int max(int a, int b, int c, int d){
    int res = a;
    if(b > res) res = b;
    if(c > res) res = c;
    if(d > res) res = d;
    return res;
}

void Router::findHeader(string packet){
    indexHeader1 = 0;
    indexHeader2 = 0;
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
}

void Router::showQueueContent(){
    cout << "Queue content: \n";
    for(string s : queue){
        findHeader(s);
        cout << s.substr(0, indexHeader2) << "\n";
    }
}

void Router::run() {
    fd_set currFd, tempFd;
    FD_ZERO(&currFd);
    int maxFd = max(fromReceiver->fd, fromSender->fd, toSender->fd, toReceiver->fd);
    FD_SET(fromSender->fd, &currFd);
    FD_SET(toSender->fd, &currFd);
    FD_SET(fromReceiver->fd, &currFd);
    FD_SET(toReceiver->fd, &currFd);
    int counter = 0;
    while (true){
        tempFd = currFd;
        select(maxFd + 1, &tempFd, NULL, NULL, NULL);
        if (FD_ISSET(fromSender->fd, &tempFd)){
        //    cout<<"Sender side"<<endl;
           string packet = fromSender->receive();
           if(queue.size() < QUEUESIZE){
               queue.push_back(packet);
            //    cout << "queu.size() " << queue.size() << "\n";
            }
        }
        else if (FD_ISSET(fromReceiver->fd, &tempFd)){
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