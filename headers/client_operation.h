#ifndef CLIENT_OPERATION_H_INCLUDED
#define CLIENT_OPERATION_H_INCLUDED

#include <termios.h>

class mailclient;

class client_operation{

friend class mailclient;

private:
    int label;
    std::string name;

    bool available = false;


protected:
    void prompt_input(const std::string& name, std::string &messsage, size_t max_len);
    void prompt_input(const std::string& name, std::string &messsage);
    std::string prompt_input_user(const std::string& name, std::string &message);
    void prompt_input_password(const std::string& name, std::string& message, bool asterisks);

    int getch();


public:
    client_operation(int label, std::string name);
    virtual ~client_operation();
    virtual std::string execute(mailclient* this_client) = 0;


};

class login_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute(mailclient* this_client);
};

class send_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute(mailclient* this_client);
};

class read_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute(mailclient* this_client);
};

class list_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute(mailclient* this_client);
};

class delete_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute(mailclient* this_client);
};

class quit_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute(mailclient* this_client);
};
#endif // CLIENT_OPERATION_H_INCLUDED
