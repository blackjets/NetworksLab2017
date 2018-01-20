//
// Created by sobieg on 19.12.17.
//

#ifndef ZHKH_CLIENT_H
#define ZHKH_CLIENT_H

#include "meter.h"

void menu(int sockfd, struct sockaddr_in* servaddr);
void init(struct sockaddr_in* addr);


int main(int, char**);


#endif //ZHKH_CLIENT_H
