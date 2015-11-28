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

void client_operation::prompt_input_password(const string& name, string& message, bool asterisks){

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
    message += password;
    message += "\n";
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

string client_operation::prompt_input_receivers(const string& name, string &message){

    string input = "";
    string receivers;
    std::regex user_regex("^([i][f][1][4][b][0])([0][1-9]|[1-7][0-9])$");

    cout << "Enter the " << name << "(Enter a . to stop):" << endl;

    while(1){
        getline(cin, input);
        if(input == "."){
            break;
        }
        if(!regex_match(input, user_regex)){
            cout << "At least one of the entered user names has an invalid format." << endl
            << "Please re-enter." << endl;
            continue;
        }
        receivers += input;
        receivers += ";";
    }

    message += receivers;
    message += "\n";

    return input;
}

string client_operation::prompt_input_user(const string& name, string &message){

    string input = "";
    std::regex user_regex("^([i][f][1][4][b][0])([0][1-9]|[1-7][0-9])$");

    cout << "Enter the " << name << ":" << endl;

    while(1){
        getline(cin, input);
        if(!regex_match(input, user_regex)){
            cout << "The entered user names has an invalid format." << endl
            << "Please re-enter." << endl;
            continue;
        }
        else{
            break;
        }
    }

    message += input;
    message += "\n";

    return input;
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

vector<string> client_operation::prompt_input_attachments(){

    string input = "";
    vector<string> filepaths;
    filepaths.clear();  //zu 90% redundant -> bestätigen
    bool attaching = true;

    cout << "Do you want to attach files to your mail? [y/n]" << endl;

    do{
        if(!input.empty()){
            cout << "There is only yes [y] or no [n]." << endl;
        }
        getline(cin, input);
    }while("y" != input && "n" != input);

    if(input == "y"){
        cout << "Specify the path to the file that you want to attach:" << endl;
        //<< "Enter . to stoqp adding files(currently only one allowed and possible)" << endl;
        while(attaching){
            getline(cin, input);
            if("." == input){
                break;
            }
            string filepath = dir_handler::make_absolute_base_path(input);
            if(filepath.empty()){
                cout << "could not find the specified file" << endl;
            }
            else{
                filepaths.push_back(filepath);
                attaching = false; //Das hier sorgt dafür, dass nur eine Datei angehängt werden kann
            }
        }

    }

    return filepaths;
}

string login_operation::execute(mailclient* this_client){

    string message = "LOGIN\n";

    this_client->set_username(prompt_input_user("your Username", message));
    prompt_input_password("Password", message, true);

    message += ".\n";

    this_client->send_all(message);
    string answer;
    this_client->receive_answer(answer);

    cout << answer;

    return message;
}

string send_operation::execute(mailclient* this_client){

    if(false ==this_client->user_logged_in()){
        cout << "You need to login first." << endl;
        return "LOGIN_ERR";
    }

    string message = "SEND\n";

    //prompt_input_user("Sender", message);
    message += this_client->get_username();
    message += "\n";
    prompt_input_receivers("Receivers", message);
    prompt_input("Subject", message, 80);
    prompt_input("Content of your message", message);
    auto filepaths = prompt_input_attachments();
    vector<int> filesizes;

    /**
        filesize, filename, binary
    */

    message += ".\n";

    int attachment_count = filepaths.size();

    message += to_string(attachment_count);
    message += "\n";

    if(attachment_count > 0){
        struct stat buf;
        int status;

        for( auto filepath : filepaths){
            const char* c_path = new char[filepath.length()+1];
            c_path = filepath.c_str();

            if(stat(c_path, &buf)== -1){
                cerr << "failed to get status info on file " << filepath << endl;
            }
            //find filename
            string filename = filepath.substr(filepath.rfind("/")+1);
            cout << "TEST: " << filename << endl;

            message += filename;
            message += "\n";

            //find filesize
            int filesize = buf.st_size;
            filesizes.push_back(filesize);

            message += to_string(filesize);
            message += "\n";

        }

    }

    message += ".\n";

    /**

    Form ohne Attachment:

    <SENDER>\n
    <RECEIVER>;<RECEIVER>;<RECEIVER>;<RECEIVER>;\n
    <SUBJECT>\n
    <MESSAGE>\n
    .\n
    <NUMBER OF ATTACHMENTS>\n
    .\n

    Form mit Attachment:

    <SENDER>\n
    <RECEIVER>;<RECEIVER>;<RECEIVER>;<RECEIVER>;\n
    <SUBJECT>\n
    <MESSAGE>\n
    .\n
    <NUMBER OF ATTACHMENTS>\n
    ?<FILENAME>\n
    ?<FILESIZE>\n
    .\n

    Die binären Daten der Datei werden in einem separaten send verschickt (sendfile()).

    */

    cout << message;
    this_client->send_all(message);


    if(attachment_count > 0){
        int i = 0;
        for( auto filepath : filepaths){

            string ok;
            this_client->receive_answer(ok);
            cout << ok << endl;
            if(ok=="GO"){
                const char* c_path = new char[filepath.length()+1];
                c_path = filepath.c_str();

                FILE* fp = fopen(c_path, "rb");
                //int in_fd = fileno(fp);
                //unsigned char buffer[1024];
                //sendfile(this_client->sd, in_fd, NULL, 1024);
                cout << "sending file now: " << filepath << " with size: " << filesizes[i] << endl;
                this_client->send_file(filepath, filesizes[i]);
                //receive an answer to the data transfer

            }
            else{
                cerr << "Error during file transmission communication" << endl;
                return "ERR(communication failed)\n";
            }


        }
    }

    string answer;
    this_client->receive_answer(answer);
    cout << answer;

    return "";

    /*

    this_client->send_all(message);
    string answer;
    this_client->receive_answer(answer);
    cout << answer;

    /**
        für später:
            Jede operation weiß, welche Antwort sie erwartet.
            hier kann man die antwort parsen, in daten strukturen umwandeln, etc.
    *
    if(answer == "GOOD BYE!\n"){
        this_client->running = 0;
    }*/

}

string list_operation::execute(mailclient* this_client){

    if(false ==this_client->user_logged_in()){
        cout << "You need to login first." << endl;
        return "LOGIN_ERR";
    }

    string message = "LIST\n";

    //prompt_input_user("Sender", message);
    message += this_client->get_username();
    message += "\n";

    message += ".\n";


    //send
    this_client->send_all(message);
    //recv
    string answer;
    this_client->receive_answer(answer);
    cout << answer;

    return "";
}

string read_operation::execute(mailclient* this_client){

    if(false ==this_client->user_logged_in()){
        cout << "You need to login first." << endl;
        return "LOGIN_ERR";
    }

    string message = "READ\n";
    string mail_id;

    //prompt_input_user("Sender", message);
    message += this_client->get_username();
    message += "\n";
    prompt_input("Mailnumber", message, 10000);

    message += ".\n";

    //send
    this_client->send_all(message);
    //recv
    string answer;
    this_client->receive_answer(answer);
    cout << answer;

    return message;
}

string delete_operation::execute(mailclient* this_client){

    if(false ==this_client->user_logged_in()){
        cout << "You need to login first." << endl;
        return "LOGIN_ERR";
    }

    string message = "DEL\n";

    //prompt_input_user("Sender", message);
    message += this_client->get_username();
    message += "\n";
    prompt_input("Message number", message);

    message += ".\n";

    //send
    this_client->send_all(message);
    //recv
    string answer;
    this_client->receive_answer(answer);
    cout << answer;

    return message;
}

string quit_operation::execute(mailclient* this_client){

    /*if(false ==this_client->user_logged_in()){
        cout << "GOOD BYE!" << endl;
        this_client->stop();
        return "INSTANT_QUIT";
    }*/

    string message = "QUIT\n";

    //prompt_input_user("Sender", message);
    message += this_client->get_username();
    message += "\n";

    message += ".\n";

    //send
    this_client->send_all(message);
    //recv
    string answer;
    this_client->receive_answer(answer);
    cout << answer;

    if(answer == "GOOD BYE!\n"){
        this_client->stop();
    }

    return message;
}
