#ifndef COMMUNICATION_H_INCLUDED
#define COMMUNICATION_H_INCLUDED

#include "../headers/mails.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <list>

class client_operation;
class server_operation;

class comm{

private:

protected:
	//Constructor
	comm();

	//Socket properties
	struct sockaddr_in partner_adr;
	int stream_sd = 0;

public:
	//Destructor
	virtual ~comm();
	//Factory method
	//virtual static comm make_comm(int port, const std::string& ip_string);

	//Connection Client

    //Connection Server
    //bind
    //accept

    //Data Transfer
    int send_message(const std::string& message);
    int send_file(const std::string& filepath);
    int receive_message(std::string& buf_message);
    int receive_file(const std::string& filepath, int receiver_count, mail& new_mail);
};

class client_comm : public comm{

private:

public:
	//Constructor
	using comm::comm;
	//Factory
	static client_comm make_client_comm(int port, const std::string& ip_string);
	//Connection
	void connect_to_server();
    void receive_welcome(std::list<client_operation*>& client_op_list);
};

class server_comm : public comm{

private:
	struct sockaddr_in my_adr;
	int listen_sd;
public:
	//Constructor
	using comm::comm;
	//Destructor
	~server_comm();
	//Factory
	static server_comm make_server_comm(int port);
	//Connection
	int socket_listen();
	void send_welcome(std::list<server_operation*>& server_op_list);
	int accept_connection();

};
#endif // MAILCLIENT_H_INCLUDED
