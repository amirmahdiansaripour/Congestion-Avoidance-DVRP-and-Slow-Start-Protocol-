#include "router.hpp"

int main(int argc, char* argv[]){   //toReceiver, fromReceiver, toSender, fromSender
    std::string port = argv[1];
    // int to_Sender = atoi(argv[2]);
    // int from_Sender = atoi(argv[3]);
    // int to_Receiver = atoi(argv[4]);
    // int form_Receiver = atoi(argv[5]);
    Router router = Router(port);
    router.run();
}