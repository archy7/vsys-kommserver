#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <algorithm>
#include <termios.h> //asterisks for password
#include <list>

class mail;
class client_operation;

class UI{

private:
	//Helper
	int getch();

public:
	//Con- and Destructor
	UI();
	~UI();

	//Methods

	std::string prompt_menu_option(std::list<client_operation*>& server_op_list);
	std::string prompt_username();
	std::string prompt_password(bool asterisks);
	std::string prompt_receiver_list();
	/**
	 * returns the receiverlist in the format:
	 * 		<RECEIVER>;<RECEIVER>;<RECEIVER>;
	 */
	std::string prompt_subject();
	std::string prompt_content();
	std::list<std::string> prompt_attachments();
	unsigned int prompt_mail_id();
	int prompt_download_attachment();
	/**
	 * returns either -1 (ALL) or 0 (NONE) or a positive integer
	 */
	void print_mail(const mail& this_mail);
};

#endif

