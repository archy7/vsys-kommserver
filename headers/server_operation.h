#ifndef SERVER_OPERATION_H_INCLUDED
#define SERVER_OPERATION_H_INCLUDED

#include "../headers/dir_handler.h"

class server_operation{

private:
    int label;
    std::string name;
public:
    server_operation(int label, std::string name);
    virtual ~server_operation();
    virtual std::string execute(std::stringstream& message_stream, dir_handler& this_handler) = 0;

friend class mailserver;
};

class send_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(std::stringstream& message_stream, dir_handler& this_handler);
};

class read_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(std::stringstream& message_stream, dir_handler& this_handler);
};

class list_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(std::stringstream& message_stream, dir_handler& this_handler);
};

class delete_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(std::stringstream& message_stream, dir_handler& this_handler);
};

class quit_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(std::stringstream& message_stream, dir_handler& this_handler);
};

#endif // SERVER_OPERATION_H_INCLUDED
