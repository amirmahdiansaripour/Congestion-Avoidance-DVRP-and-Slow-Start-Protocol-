#include "receiver.hpp"

using namespace std;

int main(int argc, char* argv[]){
    string port = argv[1];
    int toRouter = atoi(argv[2]);
    int fromRouter = atoi(argv[3]);
    Receiver receiver(port, toRouter, fromRouter);   //ipadress, toRouter, fromRouter
    receiver.run();
    receiver.reconstructFile();
}