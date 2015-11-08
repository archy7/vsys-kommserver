#include "../headers/mailserver.h"
#include "../headers/server_operation.h"
#include "../headers/dir_handler.h"

using namespace std;

/*volatile sig_atomic_t running = 1;

void shutdown(int sig){

    (void)sig;
	running = 0;
}*/

mailserver::mailserver(int port, string directory){

    this->directory = directory;

    if((this->listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cerr << "Problem opening listening socket. Exiting" << endl;
        exit(1);
    }

    memset(&this->my_adress, 0, sizeof(this->my_adress));
    this->my_adress.sin_addr.s_addr = htonl (INADDR_ANY);
    this->my_adress.sin_port = htons(port);
    this->my_adress.sin_family = AF_INET;
    //this->my_adress.sin_zero = nullptr;

    if(bind(this->listen_sd, reinterpret_cast<const sockaddr*>(&this->my_adress), sizeof(this->my_adress)) == -1){ //potenzielles Problem bei zweitem Parameter
        cerr << "Problem binding listening socket. Exiting" << endl;
        exit(1);
    }

    //shared_ptr<server_operation> temp_ptr;

    //temp_ptr = make_shared<send_operation>(1, "SEND");
    this->op_list.push_back(new login_operation(1, "LOGIN"));
    this->op_list.push_back(new send_operation(2, "SEND"));
    this->op_list.push_back(new list_operation(3, "LIST"));
    this->op_list.push_back(new read_operation(4, "READ"));
    this->op_list.push_back(new delete_operation(5, "DEL"));
    this->op_list.push_back(new quit_operation(6, "QUIT"));
}

mailserver::~mailserver(){

    if(close(this->listen_sd)==-1){
        cerr << "Problem closing listening socket." << endl;
    }

    /**
        MEMORY DER LISTE DELETEN
        smart pointer wollten nicht
    */
    for(auto ptr : this->op_list)
        delete ptr;
}

void mailserver::run(){

    listen(this->listen_sd, 500);
    socklen_t addr_len = sizeof(this->client_adr);

    string message;

    /**
        for( auto this_robot: robot_vec ){
            auto find_lambda = [this_robot](){this_robot->find_exit();};
            thread_vec.push_back(std::make_shared<std::thread>(find_lambda));
    }

    */


    while(1){
        cout << "listening..." << endl;
        int stream_sd;
        if((stream_sd = accept(this->listen_sd, reinterpret_cast<sockaddr*>(&this->client_adr), &addr_len)) == -1){
            cerr << "error accepting connection." << endl;
        }
        else{
            auto communicate_lambda = [this, stream_sd](){this->communicate(stream_sd);};
            this->server_threads.push_back(make_shared<std::thread>(communicate_lambda));
        }

    }
    close(this->listen_sd);
}

int mailserver::send_all(int stream_sd, const string& message){

    unsigned int total = 0;
    int bytes_left = message.length();
    int bytes_sent = 0;

    const char * c_message = message.c_str();

    while(total < message.length()){
        bytes_sent = send(stream_sd, c_message+total, bytes_left, 0);
        if(-1 == bytes_sent ){
            cerr << "Error while sending a message to the client." << endl;
            return -1;
        }
        total += bytes_sent;
        bytes_left -= bytes_sent;
    }
    return total;
}

void mailserver::welcome_client(int stream_sd){

    cout << "Connection accepted from " << inet_ntoa (this->client_adr.sin_addr) << ":" << ntohs(this->client_adr.sin_port) << endl;
    string message;

    for(auto ptr : this->op_list){
        //strcat(message, ptr->name.c_str());
        //strcat(message, "\n");
        message += ptr->name;
        message += "\n";

    }

    message += ".\n";
    send_all(stream_sd, message);
}

int mailserver::receive_message(int stream_sd, string& message){

    char dummy[MSG_BUF];

    //first peek inside and see how long the message is
    int message_size=recv(stream_sd,dummy,MSG_BUF-1, MSG_PEEK);
    if(0 == message_size){
        cerr << "Remote socket was closed" << endl;
        return 0;
    }
    if(-1 == message_size){
        cerr << "Error receiving message" << endl;
        return -1;
    }
    //allocate memory accordingly
    char * c_message= new char [message_size+1];
    //again check for the length of the message.
    int double_check=recv(stream_sd, c_message, message_size, 0);
    /*if(double_check != message_size){
        cerr << "Something went horribly wrong in mailserver::receive_message()" << endl;
    }*/

    if (message_size>0){
        c_message[message_size]= '\0';
        message = c_message;
        /*cout << "Client sent:" << endl;
        cout << c_message;
        cout << message;*/
    }
    delete[] c_message;
    return message_size;
}

void mailserver::communicate(int stream_sd){

    this->welcome_client(stream_sd);

    /**
        Here the actual communication between server and client takes place.
        The server receives requests by the client and acts accordingly.
        He does so by reading the first line of the incoming request. (as it is specified in the protocol)
    */

    dir_handler this_dir_handler = dir_handler::make_dir_handler(this->directory);

    do{
        string message;
        receive_message(stream_sd, message);
        stringstream message_stream(message);
        string op_wish;
        getline(message_stream, op_wish);

        string response;
        bool op_found = false;
        for(auto ptr : this->op_list){
            if(op_wish == ptr->name){
                response = ptr->execute(message_stream, this_dir_handler);
                op_found = true;
                break;
            }
        }
        if(false == op_found){
            response = "ERR(desired operation not avaiable)\n";
        }
        send_all(stream_sd, response);
        if(response == "GOOD BYE!\n"){
            break;
        }

    }while(/*running*/1);

     close (stream_sd);
}
