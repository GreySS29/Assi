#pragma once
#include <string>
#include <exception>


class InputException : public std::exception{

    private:
    std::string message;

    public:
    InputException(const std::string& msg) : message(msg){}
    const char* waht() const noexcept override {
        return message.c_str();
    }

}


