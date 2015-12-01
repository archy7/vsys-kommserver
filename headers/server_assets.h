#ifndef SERVER_ASSETS_H_INCLUDED
#define SERVER_ASSETS_H_INCLUDED

#include "../headers/dir_handler.h"
#include "../headers/mails.h"

class server_assets{

friend class mailserver;

private:
	//Constructor
	server_assets();

	//Properties
	bool server_running = true;
	std::string username = "";
	std::list<mail> mail_list;

public:
	//Destructor
	~server_assets();

	//Assets
	dir_handler my_handler;
};

#endif
