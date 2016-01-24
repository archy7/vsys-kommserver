#ifndef CLIENT_OPERATION_H_INCLUDED
#define CLIENT_OPERATION_H_INCLUDED

#include "../headers/client_assets.h"
#include "../headers/communication.h"


//#include <sys/stat.h>
//#include <stdio.h>

class client_operation{

friend class client_comm;
friend class mailclient;
friend class UI;

private:
    int label;
    std::string name;

    bool available = false;

public:

    client_operation(int label, std::string name);
    virtual ~client_operation();
    virtual void execute(client_assets& my_assets, client_comm& my_comm) = 0;

    //std::string get_name();
    //void make_available();

};

class c_login_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, client_comm& my_comm);

};

class c_send_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, client_comm& my_comm);

};

class c_read_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, client_comm& my_comm);

};

class c_list_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, client_comm& my_comm);

};

class c_delete_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, client_comm& my_comm);

};

class c_quit_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, client_comm& my_comm);

};
#endif // CLIENT_OPERATION_H_INCLUDED
