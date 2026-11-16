#pragma once

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

class Logger {
private:
    std::string date;
    std::string path;
    std::ofstream log;

public:
    Logger() {
        set_path();
        log.open(path);
    }

    void set_date() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};
    localtime_r(&time, &local_time);

    std::ostringstream stream;
    stream << std::put_time(&local_time, "%d.%m.%Y");

    date = stream.str();
}

    void set_path() {
        set_date();
        path = "../folder/" + date + ".txt";
    }
};