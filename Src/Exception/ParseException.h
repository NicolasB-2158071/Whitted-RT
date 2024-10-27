#pragma once

#include <exception>
#include <string>

class ParseException : public std::exception {

public:
    ParseException(const char* msg) : m_message{ msg } {}

    const char* what() const throw() {
        return m_message.c_str();
    }

private:
    std::string m_message;
};
