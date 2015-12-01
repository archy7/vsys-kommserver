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

/*class mailserver{

private:
	//Constructor
	mailserver();
	//Serveroperation = Controller
	std::list<server_operation*> op_list;
	//Threads
	std::list<std::shared_ptr<std::thread>> server_threads;

	//Communication and Assets
	comm my_comm;
	server_assets my_assets;

public:
	//Destructor
	~mailserver();
	//Factory
	static mailserver make_mailserver(int port, const std::string directory);

	//Methods
	void start();

};*/

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

    void communicate(int stream_sd);
public:
    mailserver(int port, std::string directory);
    ~mailserver();

    void run();
    int receive_message(int stream_sd, std::string& message);
    int receive_file(int stream_sd, std::string& filepath, int filesize);
    int send_all(int stream_sd, const std::string& message);

};

/*class mailclient{

private:
	//Constructor
	mailclient();
	//Clientoperationen = Controller
	std::list<client_operation*> op_list;

	//Communication and Assets
	comm my_comm;
	client_assets my_assets;

	//Properties
	bool running = 1;

public:
	//Destructor
    ~mailclient();
    //Factory-Method
    static mailclient make_mailclient(int port, const std::string& ip_string);

    //Methods
    void run();
    void stop();
    void communicate();

    //Getters and Setters
    void set_username(std::string username);
    std::string get_username();

};*/

#endif // MAILSERVER_H_INCLUDED
