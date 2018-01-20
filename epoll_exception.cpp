//
// Created by sobieg on 24.12.17.
//

#include "epoll_exception.h"

epoll_exception::epoll_exception(std::string str, int err) {
    wh+=str;
    wh+=" ";
    wh+=strerror(err);
    error = err;
}
