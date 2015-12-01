#ifndef COMMUNICATION_H_INCLUDED
#define COMMUNICATION_H_INCLUDED

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <list>

class client_operation;

class comm{

private:

	//Socket properties
	struct sockaddr_in my_adr;
	struct sockaddr_in partner_adr;
	int sd;

public:
	//Constructor
	comm();
	//Destructor
	~comm();
	//Factory method
	static comm make_communication(int port, const std::string& ip_string);

	//Connection Client
	void connect_to_server();
    void receive_welcome(std::list<client_operation*>& server_op_list);
    //Connection Server
    //bind
    //accept

    //Data Transfer
    int send_message(const std::string& message);
    int send_file(const std::string& filepath);
    int receive_message(std::string& buf_message);
    int receive_file();
};

#endif // MAILCLIENT_H_INCLUDED
