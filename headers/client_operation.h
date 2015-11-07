#ifndef CLIENT_OPERATION_H_INCLUDED
#define CLIENT_OPERATION_H_INCLUDED

#include <termios.h>

class client_operation{

friend class mailclient;

private:
    int label;
    std::string name;

    bool available = false;


protected:
    void prompt_input(const std::string& name, std::string &messsage, size_t max_len);
    void prompt_input(const std::string& name, std::string &messsage);
    void prompt_input_user(const std::string& name, std::string &message);

    int getch();
    std::string getpass(const std::string& name, bool asterisks);

public:
    client_operation(int label, std::string name);
    virtual ~client_operation();
    virtual std::string execute() = 0;


};

class send_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute();
};

class read_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute();
};

class list_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute();
};

class delete_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute();
};

class quit_operation : public client_operation{

public:
    using client_operation::client_operation;
    std::string execute();
};
#endif // CLIENT_OPERATION_H_INCLUDED
