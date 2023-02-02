#include "logger.hpp"

using namespace std;

Logger::Logger(){

}


void Logger::record(string s){
    std::time_t eventTime;
    std::time(&eventTime);
    string data = s + " at " + ctime(&eventTime);
    ofstream logStream("log.txt", std::ios::app);
    logStream << data << "\n";
    logStream.close();
}