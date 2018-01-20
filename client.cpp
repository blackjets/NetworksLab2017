#include <iomanip>
#include <vector>
#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <time.h>
#include <ctime>
#include <arpa/inet.h>
#include <cmath>
#include "socket_exception.h"
#include "meter.h"


using namespace std;


#define BUFLEN 1024
int sock = -1;
struct sockaddr_in servaddr;
char ip[16] = "192.168.31.106";
int port = 9000;

struct tm parse_time(string time) {
    if (time.size() != 10){
        throw new exception();
    }
    struct tm to_return;
    strptime(time.c_str(), "%d.%m.%Y", &to_return);
    return to_return;
}

void init() {
    clog << "Connecting to server. IP: " << ip << " port: " << port << "...";
    int r;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <0){
        throw socket_exception("socket()", sock);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_aton(ip, &servaddr.sin_addr);

    if(connect(sock,(struct sockaddr*)&servaddr, sizeof(servaddr))<0) {
        throw socket_exception("connect()", sock);
    }
    if ((r = (int) send(sock, "user\n", 5, MSG_NOSIGNAL)) < 0) {
        throw socket_exception("send()", r);
    }
    clog << " success" << endl;
}

int send_msg(char* msg){
    int r = 0;
    size_t msglen = strlen(msg);
    r = (int) send(sock, msg, msglen, MSG_NOSIGNAL);
    if (r < 0) {
        throw socket_exception("send()", r);
    }
}

void add_new_value(){
    string name;
    string date;
    int value;
    cout << "Введите название счетчика: " << endl;
    cin >> name;
    cout << "Введите дату в формате дд.мм.гггг: " << endl;
    cin >> date;
    cout << "Введите значение: " << endl;
    cin >> value;

    char *to_send = (char*) malloc(128);
    sprintf(to_send, "add %s %s %d \n", name.c_str(), date.c_str(), value);
    cout << "Debug" << to_send;



    try {
        send_msg(to_send);
    } catch (socket_exception ex) {
        throw;
    }
}

void menu() {
    int choise;
    while (1) {
        cout << endl <<"USING:" << endl;
        if (sock == -1) {
            cout << "1 -- Подключиться" << endl;
            cout << "2 -- Изменить IP подключения к серверу" << endl;
            cout << "3 -- Изменить порт подключения к серверу" << endl;
        } else {
            cout << "1 -- Отключиться" << endl;
            cout << "2 -- Внести показания счетчика" << endl;
            cout << "3 -- Показать историю счетчиков" << endl;
        }
        cout << "0 -- Выйти" << endl;
        cin >> choise;

        if (choise == 0) {
            break;
        }
        switch (choise) {
            case 1:
                if (sock == -1) {
                    try {
                        init();
                    } catch (socket_exception ex) {
                        cerr << endl << ex.what() << endl << "Сервер не отвечает. Попробуйте изменить IP и/или порт." << endl;
                        close(sock);
                        sock = -1;
                    }
                } else {
                    close(sock);
                    sock = -1;
                }
                break;
            case 2:
                if (sock != -1) {
                    try {
                        add_new_value();//TODO: NOT IPLEMENTED
                    } catch (socket_exception ex) {
                        cerr << endl << "Данные не были отправлены: " << ex.what() << " Возможно, вы были отключены сервером."
                             << endl << "Отключение..." << endl;
                        close(sock);
                        sock = -1;
                    }
                }
                else {
                    cout << "Введите IP: ";
                    cin >> ip;
                }
                break;
            case 3:
                if (sock != -1) {
                    //get_history(); //TODO: NOT IMPLEMENTED
                    cout << "history" << endl;
                }
                else {
                    cout << "Введите port: ";
                    cin >> port;
                }
                break;
            default:
                cout << "Команда не распознана" << endl;
        }
    }
}

int main(int argc, char** argv) {
    try {
        menu();
    } catch(socket_exception ex) {
        cerr << ex.what() << endl;
        return -1;
    }
    close(sock);
    return 0;
}
