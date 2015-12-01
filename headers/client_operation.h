#ifndef CLIENT_OPERATION_H_INCLUDED
#define CLIENT_OPERATION_H_INCLUDED

#include "../headers/client_assets.h"
#include "../headers/communication.h"


//#include <sys/stat.h>
//#include <stdio.h>

class client_operation{

friend class mailclient;

private:
    int label;
    std::string name;

    bool available = false;

public:
    client_operation(int label, std::string name);
    virtual ~client_operation();
    virtual void execute(client_assets& my_assets, comm& my_comm) = 0;

    std::string get_name();
    void make_available();

};

class login_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, comm& my_comm);
};

class send_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, comm& my_comm);
};

class read_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, comm& my_comm);
};

class list_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, comm& my_comm);
};

class delete_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, comm& my_comm);
};

class quit_operation : public client_operation{

public:
    using client_operation::client_operation;
    void execute(client_assets& my_assets, comm& my_comm);
};
#endif // CLIENT_OPERATION_H_INCLUDED
