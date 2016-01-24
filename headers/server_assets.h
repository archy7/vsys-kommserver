#ifndef SERVER_ASSETS_H_INCLUDED
#define SERVER_ASSETS_H_INCLUDED

#include "../headers/dir_handler.h"
#include "../headers/mails.h"

#include <ldap.h>
//#include <lber.h>

#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define FILTER "(uid=if14b*)"
#define ANON_USER NULL		/* anonymous bind with user and pw NULL */
#define ANON_PW NULL

class server_assets{

friend class mailserver;

private:
	//Constructor
	server_assets();

	//Properties
	bool server_running = true;
	std::list<mail> mail_list;
	//std::list<user_ban> ban_list;

	//User-Poperties
	std::string username = "";
	unsigned int login_tries = 0;
	//bool user_banned = false;

public:
	//Destructor
	~server_assets();

	//Assets
	dir_handler my_handler;

	//Server methods
	void shutdown();
	//User-Methods
	std::string get_username();
	void login_user(const std::string& username);
	void logout_user();
	bool is_user_logged_in();
	//LDAP
	bool LDAP_login(const std::string& username, const std::string& password);
	//Banlist Methods
	bool is_user_banned(const std::string& username);
	void user_failed_login();
	unsigned int get_login_tries();
	void ban_user(const std::string& username);
	//Mail-list methods
	bool mail_list_empty();
	void create_mail_list(const std::string& username);
	std::list<mail> get_mail_list();
	std::string mail_list_to_message();
	//Attachment methods
	void update_attachment(const std::string& attachment_path);

};

#endif
