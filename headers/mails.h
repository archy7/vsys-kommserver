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
    unsigned int mail_id;
    std::string sender;
    std::string receiver;
    std::string subject;
    std::string content;
    std::string filename;
    std::vector<std::string> attachments;
    std::vector<int> filesizes;
    unsigned int attachment_count;

    std::string create_file_name(std::string);



public:
    mail(); //supposed to be private
    ~mail();
    static mail make_new_mail(std::string sender, std::string receiver, std::string subject, std::string content, int attachment_count, std::vector<std::string> filepaths, std::vector<int> filesizes);
    static mail make_mail_from_file(std::string filepath, std::string mail_name);
    bool save_to_file(std::string user_path);
    bool delete_file(std::string user_path) const;

    void set_receiver(std::string receiver);
    void set_mail_id(int mail_id);
    unsigned int get_mail_id() const;
    bool has_content();
    bool has_attachment(unsigned int id);
    bool has_attachments();
    unsigned int get_attachment_count();
    void set_content(std::string new_content);
    std::vector<std::string> get_attachment_names() const;
    std::string get_subject() const;
    std::string to_message() const;

friend class dir_handler;
friend class server_operation;

friend class UI;
};

#endif // MAIL_H_INCLUDED
