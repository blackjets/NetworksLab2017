//
// Created by sobieg on 19.12.17.
//

#include "socket_exception.h"
#include <exception>



socket_exception::socket_exception(std::string str, int err) {
    wh+=str;
    wh+=" ";
    wh+=strerror(err);
    error = err;
}

