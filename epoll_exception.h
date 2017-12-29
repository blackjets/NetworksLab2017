//
// Created by sobieg on 24.12.17.
//

#ifndef ZHKH_EPOLL_EXCEPTION_H
#define ZHKH_EPOLL_EXCEPTION_H

#include <stdexcept>
#include <cstring>

class epoll_exception : public std::exception {

private:
    std::string wh = "Epoll exception: ";
    int error;

public:
    epoll_exception(int err){};
    epoll_exception(std::string str, int err);

    virtual const char* what() const throw() {
        return wh.c_str();
    }
};


#endif //ZHKH_EPOLL_EXCEPTION_H
