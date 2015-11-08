#include "../headers/mailclient.h"
#include "../headers/client_operation.h"

using namespace std;

//volatile sig_atomic_t running = 1;

/*void shutdown(int sig){

    (void)sig;
	running = 0;
}*/

mailclient::mailclient(int port, string ip_string){

    if((this->sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cerr << "Problem opening socket. Exiting" << endl;
        exit(1); //unsauber
    }
    else
        cout << "opened socket" << endl;


    char* ip_chars = new char[ip_string.length()+1];
    strcpy(ip_chars, ip_string.c_str());

    memset(&this->server_adr, 0, sizeof(this->server_adr));

    inet_aton((const char*) ip_chars, &this->server_adr.sin_addr); //SERVER IP
    this->server_adr.sin_port = htons(port);
    this->server_adr.sin_family = AF_INET;

    delete ip_chars;

    this->op_list.push_back(new login_operation(1, "LOGIN"));
    this->op_list.push_back(new send_operation(2, "SEND"));
    this->op_list.push_back(new list_operation(3, "LIST"));
    this->op_list.push_back(new read_operation(4, "READ"));
    this->op_list.push_back(new delete_operation(5, "DEL"));
    this->op_list.push_back(new quit_operation(6, "QUIT"));
}

mailclient::~mailclient(){

    //Descriptors
    if(close(this->sd)==-1){
        cerr << "Problem closing socket." << endl;
    }
    else
        cout << "closed socket" << endl;

    //Memory

    for(auto ptr : this->op_list){
        delete ptr;
    }

}

void mailclient::set_username(string username){
    this->username = username;
}

string mailclient::get_username(){
    return this->username;
}

bool mailclient::user_logged_in(){
    return !this->username.empty();
}

void mailclient::connect_to_server(){ //Verbinden und Antwort des Servers erwarten

    bool connected = false;
    for(int tries = 0; connected == false; ++tries){
        if(connect(this->sd, reinterpret_cast<const struct sockaddr*>(&this->server_adr), sizeof(this->server_adr)) == -1){
            sleep(2);
            if(tries == MAX_RETRIES){
                cerr << "Connection failed after 3 retrys. Exiting." << endl;
                exit(1);
            }
        }
        else{
            connected = true;
            cout << "Connection with server " << inet_ntoa (this->server_adr.sin_addr) << ":" << ntohs(this->server_adr.sin_port) << " established." << endl;
        }
    }
}

void mailclient::run(){

    this->connect_to_server();
    this->receive_welcome();
    this->communicate();

}

void mailclient::receive_welcome(){

    char dummy[MSG_BUF];
    int message_size;
    int double_check;

    //first peek inside and watch how long the message is
    message_size=recv(this->sd,dummy,MSG_BUF-1, MSG_PEEK);
    //allocate memory accordingly
    char * c_message = new char[message_size];
    //again check for the length of the message.
    double_check=recv(this->sd, c_message, message_size, 0);
    if(double_check != message_size){
        cerr << "Something went horribly wrong in mailclient::receive_welcome()" << endl;
    }

    if (message_size>0){
        c_message[message_size]= '\0';
        //cout << c_message;
        /**
            The welcome message of the server at this point has the following format (example):
            "LOGIN\nSEND\nLIST\nREAD\nDEL\nQUIT\n.\n"

            reading as:

            LOGIN
            SEND
            LIST
            READ
            DEL
            QUIT
            .

            It must now be parsed to set the Client operations' availability accordingly.
            The QUIT operation is always available and is not offered by the server.
        */

        string message = c_message;
        stringstream ss(message); //Stringstream cannot be initialized with c-String
        string op_name;
        while(getline(ss, op_name, '\n')){
            if( "." == op_name){
                break;
            }
            for(auto ptr : this->op_list){
                if(op_name == ptr->name){
                    ptr->available = true;
                }
            }
        }

        /**
            At this point, the client is informed about what operations the server offers.
        */


    }

    delete[] c_message;
}

void mailclient::communicate(){

    while(this->running){

        cout << "Choose what you wish to do" << endl;
        for( auto ptr : this->op_list){
            cout << ptr->name << endl;
        }
        cout << ">>" << endl;

        string op_wish;
        getline(cin, op_wish);
        transform(op_wish.begin(), op_wish.end(), op_wish.begin(), ::toupper);
        for( auto ptr : this->op_list){
            if(ptr->name == op_wish){
                string message = ptr->execute(this);

                if(message=="LOGIN_ERR"){
                    cout << "You need to login first." << endl;
                    break;
                }


                if(message=="INSTANT_QUIT"){
                    cout << "GOOD BYE!" << endl;
                    this->running = 0;
                    break;
                }

                send_all(message);
                string answer;
                receive_answer(answer);
                cout << answer;

                /**
                    für später:
                        Jede operation weiß, welche Antwort sie erwartet.
                        deshalb:
                            ptr->handle_answer(answer);
                */
                if(answer == "GOOD BYE!\n"){
                    this->running = 0;
                }
                break;
            }
        }
    }
}

int mailclient::send_all(const string& message){

    unsigned int total = 0;
    int bytes_left = message.length();
    int bytes_sent = 0;

    const char * c_message = new char [message.size()+1];
    c_message = message.c_str();

    while(total < message.length()){
        bytes_sent = send(this->sd, c_message+total, bytes_left, 0);
        if(-1 == bytes_sent ){
            cerr << "Error while sending a message to the server." << endl;
            return -1;
        }
        total += bytes_sent;
        bytes_left -= bytes_sent;
    }

    //delete[] c_message;
    return total;
}

int mailclient::receive_answer(string& answer){

    char dummy[MSG_BUF];
    //first peek inside and see how long the message is
    int message_size=recv(this->sd,dummy,MSG_BUF-1, MSG_PEEK);
    if(0 == message_size){
        cerr << "Remote socket was closed" << endl;
        return 0;
    }
    if(-1 == message_size){
        cerr << "Error receiving message" << endl;
        return -1;
    }
    //allocate memory accordingly
    char * c_message = new char [message_size+1];
    //again check for the length of the message.
    int double_check=recv(this->sd, c_message, message_size, 0);
    if(double_check != message_size){
        cerr << "Something went horribly wrong in mailclient::receive_answer()" << endl;
    }

    if (message_size>0){
        c_message[message_size]= '\0';
        answer = c_message;
    }
    delete[] c_message;
    return message_size;
}

/*void mailclient::static_shutdown(int sig){
    shutdown(this->sd, SHUTRDWR);
    this->running = 0;
}*/
