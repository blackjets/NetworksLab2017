#include <unistd.h>
#include "server.h"

using namespace std;




void add_client(int is_adm){
    struct client cl;
    cl.is_admin = is_adm;
    cl.id = clients.size()+1;
    for (vector<meter>::iterator it = meters.begin(); it != meters.end(); it++) {
        meter met((*it).get_name());
        met.set_user_id(cl.id);
        cl.meters.push_back(met); //добавляем пользователю все счетчики, какие уже есть
    }
}

void add_meter(std::string name) {
    meter met(name);
    meters.push_back(met);
    for (vector<client>::iterator it = clients.begin(); it != clients.end(); it++) {
        meter met_cl(name);
        (*it).meters.push_back(met_cl); //добавляем всем пользователям новый счетчик
    }
}

vector<debts> get_debts(struct tm date) {
    vector<debts> debtors;
    for (vector<client>::iterator cl = clients.begin(); cl != clients.end(); cl++){
        //DEBUG
        clog << "Проверяем долги клиента " << (*cl).id << endl;
        //
        struct debts deb;
        int debd = 1;
        deb.cl = (*cl);
        for (vector<meter>::iterator met = (*cl).meters.begin(); met != (*cl).meters.end(); met++){
            //DEBUG
            clog << "Проверяем счетчик " << (*met).get_name() << " клиента " << (*cl).id << endl;
            //
            for (vector<value>::iterator val = (*met).get_history().begin(); val != (*met).get_history().end(); val++){
                //DEBUG
                char dat[11];
                strptime(dat, "%d.%m.%Y", &(*val).date);
                clog << "Проверяем значение на дату " << dat << " счетчика " << (*met).get_name()  << " клиента "
                     << (*cl).id << endl;
                //
                if (mktime(&(*val).date) == mktime(&date)) {
                    //DEBUG
                    clog << "Совпадение найдено" << endl;
                    //
                    debd = 0;
                    break;
                }
            }
            if (debd) {
                meter meter1 = (*met);
                deb.meters_with_debt.push_back(meter1);
            }
        }
        if (deb.meters_with_debt.size()) {
            debtors.push_back(deb);
        }
    }
    return debtors;
}


struct client_ctx g_ctxs[1+MAX_CLIENTS]; //TODO: можно переписать на вектор

int set_non_block_mode(int s) {                             //only linux
    int fl = fcntl(s, F_GETFL, 0);
    return fcntl(s, F_SETFL, fl | O_NONBLOCK);
}

int create_listening_socket(int port) {
    struct sockaddr_in addr;

    int s = socket(AF_INET, SOCK_STREAM, 0);
    /*if (s <= 0) {
        throw socket_exception("Create: ", s);
    }*/

    set_non_block_mode(s);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        //throw socket_exception("Bind: ", -1);
    }
    if (listen(s, 1) < 0) {
        //throw socket_exception("Listen: ", -1);
    }

    cout << "Listening: " << ntohs(addr.sin_port) << endl;

    return s;
}

int send_msg(struct client_ctx* client, char* msg){
    int r;
    sprintf(client->out_buff, "%s", msg);
    client->out_total = (int) strlen(client->out_buff);
    client->out_sent = 0;
    r = send(client->socket, client->out_buff, client->out_total, MSG_NOSIGNAL);
    if (r > 0) {
        client->out_sent += r;
    }
    if (client->out_sent >= client->out_total > 0) {
        cout << "Msg to client #" << client->id << " has been sent" << endl;
    }
}

void parse_msg(struct client_ctx* client) {
    struct client_ctx cl = *client;
    if (!strncmp(cl.in_buff, "add meter", 9)) {
        if (cl.is_admin == 2) {

        } else {
            send_msg(client, "You are not admin!");
        }
    }
    else if (!strncmp(cl.in_buff, "add", 3)) {
        string buff = cl.in_buff;
        string delimetr = " ";
        string tokens[4];
        size_t pos = 0;
        int i;
        for (pos = buff.find(delimetr), i = 0; pos != std::string::npos; pos = buff.find(delimetr)){
            tokens[i] = buff.substr(0, pos);
            std::cout << tokens[i] << std::endl;
            buff.erase(0, pos + delimetr.length());
        }



    }
    memset(client->in_buff, 0, sizeof(client->in_buff));
}

void epoll_serv(int port){
    int s;
    int epfd;
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS_PER_CALL];

    memset(&g_ctxs, 0, sizeof(g_ctxs));

    //try {
        s = create_listening_socket(port);
    //} catch (socket_exception ex) {
    //    throw;
    //}

    epfd = epoll_create(1);
    if (epfd <= 0) {
        //throw epoll_exception("Epoll create", errno);
    }

    ev.events = EPOLLIN;
    ev.data.fd = 0; // в data будет храниться 0 для прослушивающего сокета \
                        для другиъ -- индекс в массиве ctxs
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, s, &ev) != 0) {
        //throw epoll_exception("epoll_ctl(): ", errno);
    }

    while (1) {
        int events_cnt;

        events_cnt = epoll_wait(epfd, events, MAX_EVENTS_PER_CALL, 1000);

        if (events == 0) {
            //ничего не происходит
        }

        for (int i = 0; i<events_cnt; i++) {
            struct epoll_event* e = &events[i];
            if (e->data.fd == 0 && (ev.events & EPOLLIN)) {
                //поступило подключение
                struct sockaddr_in addr;
                socklen_t socklen = sizeof(addr);
                int as = accept(s, (struct sockaddr*) &addr, &socklen);

                if (as > 0) {
                    int j;
                    for (j = 0; j < MAX_CLIENTS; j++) {
                        if (g_ctxs[j].socket == 0) { //слот свободен, можно занять сокетом
                            memset(&g_ctxs[j], 0, sizeof(g_ctxs[j]));
                            g_ctxs[j].socket = as;
                            break;
                        }
                    }
                    if (j != MAX_CLIENTS) {
                        //Регистрация сокета клиента в общей очереди событий
                        unsigned int ip = ntohl(addr.sin_addr.s_addr);
                        set_non_block_mode(as);
                        ev.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP;
                        ev.data.fd = j;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, as, &ev);
                        cout << "New client connected: " << ((ip>>24) & 0xff) << "." << ((ip>>16)& 0xff) << "." << ((ip>>8)& 0xff) << "."
                             << (ip & 0xff) << " #" <<j << endl;
                    }
                    else {
                        //нет свободных слотов
                        close(as);
                    }
                }
            }
            if (e->data.fd >= 0) {
                int idx = e->data.fd;
                g_ctxs[idx].id = idx;
                if ((e->events & EPOLLHUP) || (e->events & EPOLLRDHUP) || (e->events & EPOLLERR)) {
                    //клиент отключился или другая ошибка
                    close(g_ctxs[idx].socket);
                    memset(&g_ctxs[idx], 0, sizeof(g_ctxs[idx]));
                    cout << "Client disconnected: " << idx << " (error or hup)" << endl;
                }
                else if ((e->events & EPOLLIN) && (g_ctxs[idx].out_total == 0) && g_ctxs[idx].received < sizeof(g_ctxs[idx].in_buff)) {
                    //пришли новые данные от клиента, если еще нет результата, то принять данные, найти результат и отправить его.
                    ssize_t r = recv(g_ctxs[idx].socket, g_ctxs[idx].in_buff, sizeof(g_ctxs[idx].in_buff), 0);
                    if (r>0) {
                        //данные приняты, будет проведен анализ
                        int k;
                        int len = -1;
                        if (g_ctxs[idx].received == 0 && g_ctxs[idx].is_admin == 0) {
                            g_ctxs[idx].is_admin = check_for_admin(g_ctxs[idx].in_buff);
                            g_ctxs[idx].received += r;
                            add_client(g_ctxs[idx].is_admin);
                            //debug
                            if (g_ctxs[idx].is_admin == 2) {
                                cout << "ADMIN" << endl;
                            }
                            else if (g_ctxs[idx].is_admin == 1) {
                                cout << "USER" << endl;
                            }
                            else if (g_ctxs[idx].is_admin == -1) {
                                close(g_ctxs[idx].socket);
                                memset(&g_ctxs[idx], 0, sizeof(g_ctxs[idx]));
                                cout << "Client disconnected: " << idx << " nor admin nor user" << endl;
                            }
                            continue;
                        }
                        g_ctxs[idx].new_msg = g_ctxs[idx].received+1;
                        g_ctxs[idx].received += r;
                        for(k = g_ctxs[idx].new_msg-1; k<g_ctxs[idx].received; k++) { //VARNING
                            if (g_ctxs[idx].in_buff[k] == '\n') {
                                len = k;
                                break;
                            }
                        }
                        if (len == -1 && k == sizeof(g_ctxs[idx].in_buff)){
                            len = k;
                        }
                        if (len != -1) {
                            parse_msg(&g_ctxs[idx]);
                            //строка получена, формирование ответа и отправка
                            //иначе продолжаем ждать данные от клиента
                        }
                        else {
                            //клиент отключился, либо возникла ошибка, закрыть соединение
                            close(g_ctxs[idx].socket);
                            memset(&g_ctxs[idx], 0, sizeof(g_ctxs[idx]));
                            cout << "Client disconnect: " << idx << " (read error)" << endl;
                        }
                    }
                    else if ((e->events & EPOLLOUT) && (g_ctxs[idx].out_total > 0)) {
                        //сокет стал готов к отправке данных, значит елси не все данные переданы, то передать \
                        если все данные были переданы, сокет можно закрыть, клиента отключить.
                        if (g_ctxs[idx].out_sent < g_ctxs[idx].out_total) {
                            int r = send(g_ctxs[idx].socket, g_ctxs[idx].out_buff + g_ctxs[idx].out_sent,
                                         g_ctxs[idx].out_total - g_ctxs[idx].out_sent, MSG_NOSIGNAL);
                            if (r > 0) {
                                g_ctxs[idx].out_sent += r;
                            }
                        }
                        if (g_ctxs[idx].out_sent >= g_ctxs[idx].out_total) {
                            cout << "Response has been sent: " << idx << endl;
                            close(g_ctxs[idx].socket);
                            memset(&g_ctxs[idx], 0, sizeof(g_ctxs[idx]));
                            cout << "Client disconnect " << idx << " (all data sent)" << endl;
                        }
                    }
                }
            }
        }
    }
}

int check_for_admin(char* str){
    if (str[0] == 'a') {
        return 2;
    }
    else if (str[0] == 'u'){
        return 1;
    }
    return -1;
}



int main(int argc, char* argv[]){
    int port = 9000;
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    epoll_serv(port);
    return 0;
}
