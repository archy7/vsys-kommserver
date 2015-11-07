#ifndef DIR_HANDLER_H_INCLUDED
#define DIR_HANDLER_H_INCLUDED

#include <list>
#include <iostream>
#include <fstream>

class mail;

#include "../headers/mails.h"

class dir_handler{
private:
    std::string base_path;
    std::list<mail> mail_list;
    dir_handler();
    static std::string make_absolute_base_path(std::string path);
public:
    ~dir_handler();
    static dir_handler make_dir_handler(std::string path);
    static bool dir_exists(std::string path);
    bool user_dir_exists(std::string path);
    void make_user_dir(std::string path); //optional -C als Startoption erstellt das Verzeichnis
    std::string make_absolute_user_path(std::string username);
    bool mail_list_is_empty();
    bool create_mail_list(std::string username);
    std::list<mail> get_mail_list();
    int get_mail_list_length() const;
    std::string list_to_message() const;
};

#endif // DIR_HANDLER_H_INCLUDED
