#ifndef CLIENT_ASSETS_H_INCLUDED
#define CLIENT_ASSETS_H_INCLUDED

#include "../headers/UI.h"
#include "../headers/dir_handler.h"
#include "../headers/mails.h"

class client_assets{

friend class mailclient;

private:
	//Constructor
	client_assets();

	//Properties
	bool client_running = true;
	std::string username = "";
	std::list<mail> mail_list;

public:
	//Destructor
	~client_assets();
	//Factory
	void initialize();

	//Assets
	UI my_UI;
	dir_handler my_handler;

	//Client-Methods
	void shutdown();
	//User-Methods
	bool user_logged_in();
	void set_username(std::string username);
	std::string get_username();
	//Maillist-Methods
	bool mail_list_empty();
	void build_mail_list(std::string mailstring);
	void clear_mail_list();
	bool mail_exists(unsigned int id);
	mail find_mail_by_id(unsigned int id);

};

#endif
