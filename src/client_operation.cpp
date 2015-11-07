#include "../headers/mailclient.h"
#include "../headers/client_operation.h"

using namespace std;

client_operation::client_operation(int label, string name){
    this->label = label;
    this->name  = name;
    //    this->user_regex =  = ;
}

client_operation::~client_operation(){

}

int client_operation::getch(){

    int ch;
    struct termios t_default, t_hidden;

    tcgetattr(STDIN_FILENO, &t_default);
    t_hidden = t_default;
    t_hidden.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_hidden);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_default);

    return ch;
}

string client_operation::getpass(const string& name, bool asterisks){

    const char BACKSPACE = 127;
    const char RETURN = 10;

    string password;
    unsigned char ch=0;

    cout << name << ":" << endl;

    while(RETURN!=(ch=client_operation::getch())){
        if(BACKSPACE == ch){
            if(0!=password.length()){
                if(asterisks){
                    cout <<"\b \b";
                }
                password.resize(password.length()-1);
            }

        }
        else{
            password += ch;
            if(asterisks){
                cout << '*';
            }
        }
    }

    cout << endl;
    return password;

}

void client_operation::prompt_input(const string& name, string &message, size_t max_len){

    string input = "";

    while(input.empty() || input.length() > max_len){
        if(input.empty())
            cout << "Enter the " << name << ":" << endl;
        else if(input.size() > max_len)
            cout << "The entered " << name << " is too long (maximum of " << max_len << " characters)" << endl
            << "Please re-enter." << endl;
        else
            cerr << "Something went horribly wrong in client_operation::prompt_input -> "<< name << ". LIKE REALLY WRONG" << endl;

        getline(cin, input);
    }
    message += input;
    message += "\n";
}

void client_operation::prompt_input_user(const string& name, string &message){

    string input = "";
    std::regex user_regex("^([i][f][1][4][b][0])([0][1-9]|[1-7][0-9])$");

    while(!regex_match(input, user_regex)){
        if(input.empty())
            cout << "Enter the " << name << ":" << endl;
        else
            cout << "The entered user name has an invalid format." << endl
            << "Please re-enter." << endl;

        getline(cin, input);
    }

    message += input;
    message += "\n";
}

void client_operation::prompt_input(const string& name, string &message){

    string input = "";

    cout << "Enter the " << name << ":" << endl;

    while(input.empty() || input != "."){
        //if(input.empty())
        getline(cin, input);
        if(input == ".")
            break;

        message += input;
        message += "\n";
        input.clear();
    }

}

string send_operation::execute(){

    string message = "SEND\n";

    prompt_input_user("Sender", message);
    prompt_input_user("Receiver", message);


    {
        string testpass = "null";
        string password = this->getpass("Password", true);

        if(testpass == password)
            cout << "OH YEAHHHHHH" << endl;
        else
            cout << "OHH NOOOESSS" << endl;
    }

    prompt_input("Subject", message, 80);
    prompt_input("Content of your message", message);

    message += ".\n";

    return message;

}

string list_operation::execute(){

    string message = "LIST\n";

    prompt_input_user("Username", message);

    message += ".\n";

    return message;
}

string read_operation::execute(){

    string message = "READ\n";
    string mail_id;

    prompt_input_user("Username", message);
    prompt_input("Mailnumber", message, 10000);

    message += ".\n";

    return message;
}

string delete_operation::execute(){

    string message = "DEL\n";

    prompt_input_user("Username", message);
    prompt_input("Message number", message);

    message += ".\n";

    return message;
}

string quit_operation::execute(){

    string message = "QUIT\n";

    message += ".\n";

    return message;
}
