#include "sender.hpp"

using namespace std;
int main(int argc, char* argv[]){   //ip, toRouter, fromRouter
    string port = argv[1];
    string destPort = argv[2];
    // int toRouter = atoi(argv[2]);
    // int fromRouter = atoi(argv[3]);
    Sender sender = Sender(port, destPort);
    sender.recordData();
}