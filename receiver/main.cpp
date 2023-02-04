#include "receiver.hpp"

using namespace std;

int main(int argc, char* argv[]){
    string port = argv[1];
    // string destPort = argv[2];
    Receiver receiver(port);   //ipadress, toRouter, fromRouter
    receiver.reconstructFile();
}