#include "logger.hpp"

using namespace std;

Logger::Logger(){

}

void Logger::add(string s){
    std::time_t eventTime;
    std::time(&eventTime);
    string d = s + " at " + ctime(&eventTime);
    data.push_back(d);
}

void Logger::finalWrite(){
    ofstream logStream("log.txt", std::ios::app);
    for(int i = 0; i < data.size(); i++){
        logStream << data[i] << "\n";
    }
    logStream.close();
}

void Logger::record(string s){
    std::time_t eventTime;
    std::time(&eventTime);
    string data = s + " at " + ctime(&eventTime);
    ofstream logStream("log.txt", std::ios::app);
    logStream << data << "\n";
    logStream.close();
}