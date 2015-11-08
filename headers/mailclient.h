#ifndef MAILCLIENT_H_INCLUDED
#define MAILCLIENT_H_INCLUDED

/**
    Header für den Mailclient

*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
#include <getopt.h>
#include <string>
#include <list>
#include <sstream>
#include <algorithm>
#include <assert.h>
#include <stdlib.h>
#include <signal.h>

#include <regex>

#include "../headers/client_operation.h"
#include "../headers/dir_handler.h"
#include "../headers/mails.h"

#define MSG_BUF 1024
#define MAX_RETRIES 3

//void shutdown(int sig);

class mailclient{

private:
    //Clientoperationen
    std::list<client_operation*> op_list;

    std::string username = "";

    //Socketeigenschaften
    int sd;
    struct sockaddr_in server_adr;
    int send_all(const std::string& message);
    int receive_answer(std::string& answer);

    bool running = 1;

    void connect_to_server();
public:
    mailclient(int port, std::string ip_string);
    ~mailclient();

    void run();
    void communicate();
    void receive_welcome();

    void set_username(std::string username);
    std::string get_username();
    bool user_logged_in();
    //static void static_shutdown(int sig);
};

#endif // MAILCLIENT_H_INCLUDED
