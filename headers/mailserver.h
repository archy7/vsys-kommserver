#ifndef MAILSERVER_H_INCLUDED
#define MAILSERVER_H_INCLUDED

/**
    Header Datei für den Mailserver

*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <getopt.h>
#include <string>
#include <sstream>
#include <string.h> //memset();
#include <signal.h>
#include <assert.h>
#include <stdlib.h>
//#include <vector>
#include <list>
#include <memory>
#include <thread>
#include <dirent.h> //Verzeichnis-funktionen
#include <sys/stat.h>

#include "../headers/server_operation.h"

#define MSG_BUF 1024

void shutdown(int sig);

class mailserver{

private:
    //Serveroperationen
    std::list<server_operation*> op_list;
    std::list<std::shared_ptr<std::thread>> server_threads;

    //Server-Socket-Eigenschaften
    int listen_sd;
    struct sockaddr_in my_adress;
    struct sockaddr_in client_adr;

    std::string directory;

    void welcome_client(int stream_sd);
    int receive_message(int stream_sd, std::string& message);
    int send_all(int stream_sd, const std::string& message);
    void communicate(int stream_sd);
public:
    mailserver(int port, std::string directory);
    ~mailserver();

    void run();

};

#endif // MAILSERVER_H_INCLUDED
