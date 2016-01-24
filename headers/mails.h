#ifndef MAIL_H_INCLUDED
#define MAIL_H_INCLUDED

#include "../headers/dir_handler.h"

#include <ctime>
#include <iomanip>
#include <algorithm>
#include <random>
#include <sstream>
#include <pthread.h>

class mail{
private:
    int mail_id;
    std::string sender;
    std::string receiver;
    std::string subject;
    std::string content;
    std::string filename;
    std::list<std::string> attachment_paths;
    unsigned int attachment_count; //vermutlich nicht gebraucht

    std::string create_file_name(std::string);

public:
    mail(); //supposed to be private
    ~mail();
    static mail make_new_mail(std::string sender, std::string receiver, std::string subject, std::string content, int attachment_count);
    static mail make_list_mail_from_file(std::string filepath, std::string file_name);
    bool save_to_file(std::string user_path);
    bool delete_file(std::string user_path) const;

    void set_receiver(std::string receiver);
    void set_mail_id(int mail_id);
    int get_mail_id() const;
    bool has_content();
    bool has_attachment(unsigned int id);
    bool has_attachments();
    unsigned int get_attachment_count() const;
    void set_content(std::string new_content);
    std::list<std::string> get_attachment_paths() const;
    std::list<std::string> get_attachment_names() const;
    std::string get_sender() const;
    std::string get_subject() const;
    std::string get_content() const;
    std::string to_message() const;

    void add_attachment_path(std::string attachment_path);

friend class dir_handler;
friend class server_operation;

friend class UI;
};

#endif // MAIL_H_INCLUDED
