#pragma once

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

class Logger {
private:
    std::string date_;
    std::string path_;
    std::ofstream log_;
    //std::string message_;

public:
    Logger() {
        set_path();
        log_.open(path_, std::ios::app); 
    }

    void set_date() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};
    localtime_r(&time, &local_time);

    std::ostringstream stream;
    stream << std::put_time(&local_time, "%d.%m.%Y");

    date_ = stream.str();
}

    void set_path() {
        set_date();
        path_ = "/home/greys/Documents/Doc/C+/my_progect/ai/History/" + date_ + ".txt";
    }

    void write_message(const std::string& message , bool is_me) {
        log_ << (is_me ? "Me: " : "Assi: ") << message << '\n';
       log_.flush(); 
       
    }
};