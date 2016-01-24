#ifndef DIR_HANDLER_H_INCLUDED
#define DIR_HANDLER_H_INCLUDED

#include <list>
#include <iostream>
#include <fstream>

#include <unistd.h>

class mail;

#include "../headers/mails.h"

#define CLIENT 1
#define SERVER 2

class dir_handler{
private:
	//Properties
	int type =-1;
	std::string base_path = "";

public:
	//Constructor
	dir_handler(); //supposed to be private.
	//Destructor
	~dir_handler();
	//Factory
	static dir_handler make_server_handler(std::string dir_name);
	static dir_handler make_client_handler();

	//static helpers
	static bool dir_exists(std::string path);
	static std::string make_absolute_path(std::string filename);
	//static std::string string_from_c_string(char* c_string);
	static std::string filename_from_path(const std::string& full_path);
	static std::string create_attachment_file_name(std::string filename);

	//server helpers
	bool user_dir_exists(std::string path);
	void make_user_dir(std::string path);
	std::string make_absolute_attachment_file_path(std::string filename);
	std::string make_absolute_attachment_dir_path();
	std::string make_absolute_user_dir_path(std::string username);
	void update_attachment(std::string attachment_path);
};

/*class dir_handler{
private:
    std::string base_path;
    std::list<mail> mail_list; //FÄLLT WEG
    dir_handler();

    std::string create_attachment_name(std::string filename);

    std::string username = "";
public:
    ~dir_handler();
    static dir_handler make_dir_handler(std::string path);
    static bool dir_exists(std::string path);
    bool user_dir_exists(std::string path);
    void make_user_dir(std::string path); //optional -C als Startoption erstellt das Verzeichnis
    static std::string make_absolute_base_path(std::string path);
    std::string make_absolute_attachment_path(std::string filename);
    std::string make_absolute_user_path(std::string username);
    bool mail_list_is_empty(); //FÄLLT WEG
    bool create_mail_list(std::string username); //FÄLLT WEG
    std::list<mail> get_mail_list(); //FÄLLT WEG
    int get_mail_list_length() const; //FÄLLT WEG
    std::string list_to_message() const; //FÄLLT VORERST WEG

    void update_attachment(std::string attachment_path);

    void set_username(std::string username); //fällt weg
    std::string get_username(); //fällt weg
    bool user_logged_in(); 	//fällt weg
};*/


#endif // DIR_HANDLER_H_INCLUDED
