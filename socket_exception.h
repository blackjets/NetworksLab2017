//
// Created by sobieg on 19.12.17.
//

#ifndef ZHKH_SOCKET_EXCEPTION_H
#define ZHKH_SOCKET_EXCEPTION_H


#include <stdexcept>
#include <cstring>


class socket_exception : public std::exception {

private:
    std::string wh = "Socket exception: ";
    int error;

public:
    socket_exception(int err){};
    socket_exception(std::string str, int err);

    virtual const char* what() const throw() {
        return wh.c_str();
    }
};






#endif //ZHKH_SOCKET_EXCEPTION_H
