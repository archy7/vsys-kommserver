#ifndef SERVER_OPERATION_H_INCLUDED
#define SERVER_OPERATION_H_INCLUDED

/*#include "../headers/dir_handler.h"
#include "../headers/server_assets.h"
#include "../headers/communication.h"


#include <string.h>
#include <stdlib.h>
#include <ldap.h>
//#include <lber.h>*/

#include "../headers/server_assets.h"
#include "../headers/communication.h"

class server_operation{

friend class server_comm;
friend class mailserver;

private:
    int label;
    std::string name;
public:
    server_operation(int label, std::string name);
    virtual ~server_operation();
    virtual void execute(server_assets& my_assets, server_comm& my_comm, std::stringstream& message_stream) = 0;

    std::string get_name();
};

class s_login_operation : public server_operation{
public:
    using server_operation::server_operation;
    void execute(server_assets& my_assets, server_comm& my_comm, std::stringstream& message_stream);
};

class s_send_operation : public server_operation{
public:
    using server_operation::server_operation;
    void execute(server_assets& my_assets, server_comm& my_comm, std::stringstream& message_stream);
};

class s_read_operation : public server_operation{
public:
    using server_operation::server_operation;
    void execute(server_assets& my_assets, server_comm& my_comm, std::stringstream& message_stream);
};

class s_list_operation : public server_operation{
public:
    using server_operation::server_operation;
    void execute(server_assets& my_assets, server_comm& my_comm, std::stringstream& message_stream);
};

class s_delete_operation : public server_operation{
public:
    using server_operation::server_operation;
    void execute(server_assets& my_assets, server_comm& my_comm, std::stringstream& message_stream);
};

class s_quit_operation : public server_operation{
public:
    using server_operation::server_operation;
    void execute(server_assets& my_assets, server_comm& my_comm, std::stringstream& message_stream);
};

#endif // SERVER_OPERATION_H_INCLUDED
