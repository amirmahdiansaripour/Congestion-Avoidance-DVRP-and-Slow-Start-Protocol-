#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <ctime>
#include <fstream>
#include <string>
#include <vector>

const std::string LOG_FILE_PATH = "log.txt";

class Logger{
public:
    Logger();
    void record(std::string);
    void add(std::string);
    void finalWrite();
    // static void logUserLogin(User* user);
    // static void logDownload(User* user, std::string fileName);
    // static void logUpload(User* user, std::string fileName);
    // static void logUserLogout(User* user);
private:
    std::vector<std::string> data;
    static void write(std::string);
    static std::time_t getTime();
};

#endif