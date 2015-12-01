#ifndef MAILCLIENT_H_INCLUDED
#define MAILCLIENT_H_INCLUDED

#include <string>
#include <list>

#include "../headers/client_operation.h"
#include "../headers/communication.h"
#include "../headers/client_assets.h"

class mailclient{

private:
	//Constructor
	mailclient();
	//Clientoperationen = Controller
	std::list<client_operation*> op_list;

	//Communication and Assets
	comm my_comm;
	client_assets my_assets;

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

};

#endif // MAILCLIENT_H_INCLUDED
