//
// Created by sobieg on 19.12.17.
//

#ifndef ZHKH_SERVER_H
#define ZHKH_SERVER_H

#include "socket_exception.h"
#include "epoll_exception.h"
#include "meter.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <vector>

#define MAX_EVENTS_PER_CALL (16)
#define MAX_CLIENTS (256)


struct client {
    std::string name;
    int is_admin;
    unsigned long id;
    std::vector<meter> meters;
};

struct debts {
    struct client cl;
    std::vector<meter> meters_with_debt;
};

struct client_ctx {
    int socket;
    char in_buff[512];
    int received;
    int new_msg;
    char out_buff[512];
    int out_total;
    int out_sent;
    int is_admin;
    int id;
};

std::vector<struct client> clients;
std::vector<meter> meters;

//сеть
int set_non_block_mode(int);
int create_listening_socket(int);
void epoll_serv(int port);
int send_msg(struct client_ctx* client, char* msg);
int check_for_admin(char*);


//внутренности
void add_client(int is_adm);
void add_meter(std::string name);


void parse_msg(struct client_ctx* client);


int main(int, char**);





#endif //ZHKH_SERVER_H
