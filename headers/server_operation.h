#ifndef SERVER_OPERATION_H_INCLUDED
#define SERVER_OPERATION_H_INCLUDED

#include "../headers/dir_handler.h"

#include <string.h>
#include <stdlib.h>
#include <ldap.h>
//#include <lber.h>

#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define FILTER "(uid=if14b*)"
#define ANON_USER NULL		/* anonymous bind with user and pw NULL */
#define ANON_PW NULL


class mailserver;

class server_operation{

friend class mailserver;

private:
    int label;
    std::string name;
public:
    server_operation(int label, std::string name);
    virtual ~server_operation();
    virtual std::string execute(mailserver* this_server, int stream_sd, std::stringstream& message_stream, dir_handler& this_handler) = 0;


};

class login_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(mailserver* this_server, int stream_sd, std::stringstream& message_stream, dir_handler& this_handler);
};

class send_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(mailserver* this_server, int stream_sd, std::stringstream& message_stream, dir_handler& this_handler);
};

class read_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(mailserver* this_server, int stream_sd, std::stringstream& message_stream, dir_handler& this_handler);
};

class list_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(mailserver* this_server, int stream_sd, std::stringstream& message_stream, dir_handler& this_handler);
};

class delete_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(mailserver* this_server, int stream_sd, std::stringstream& message_stream, dir_handler& this_handler);
};

class quit_operation : public server_operation{
public:
    using server_operation::server_operation;
    std::string execute(mailserver* this_server, int stream_sd, std::stringstream& message_stream, dir_handler& this_handler);
};

#endif // SERVER_OPERATION_H_INCLUDED
