#include "../headers/communication.h"
#include "../headers/client_operation.h"

#define MAX_RETRIES 3
#define MSG_BUF 1024

#include <iostream> //cin,cout,cerr
#include <string.h> //meset, strcpy
#include <sstream>


using namespace std;

comm::comm(){

}

comm::~comm(){

	//TODO: throw exception when closing fails
    if(close(this->sd)==-1){
        cerr << "Problem closing socket." << endl;
    }
}

comm comm::make_communication(int port, const string& ip_string){

	comm r_comm;

	//TODO: throw exception when opening fails

    if((r_comm.sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cerr << "Problem opening socket. Exiting" << endl;
        exit(1); //unsauber -
    }
    else
        cout << "opened socket" << endl;


    char* ip_chars = new char[ip_string.length()+1];
    strcpy(ip_chars, ip_string.c_str());

    memset(&r_comm.partner_adr, 0, sizeof(r_comm.partner_adr));

    inet_aton((const char*) ip_chars, &r_comm.partner_adr.sin_addr); //SERVER IP
    r_comm.partner_adr.sin_port = htons(port);
    r_comm.partner_adr.sin_family = AF_INET;

    delete ip_chars;

    return r_comm;
}

void comm::connect_to_server(){

	//TODO: throw exception when connection fails

    bool connected = false;
    for(int tries = 0; connected == false; ++tries){
        if(connect(this->sd, reinterpret_cast<const struct sockaddr*>(&this->partner_adr), sizeof(this->partner_adr)) == -1){
            sleep(2);
            if(tries == MAX_RETRIES){
                cerr << "Connection failed after 3 retrys. Exiting." << endl;
                exit(1);
            }
        }
        else{
            connected = true;
            cout << "Connection with server " << inet_ntoa (this->partner_adr.sin_addr) << ":" << ntohs(this->partner_adr.sin_port) << " established." << endl;
        }
    }
}

void comm::receive_welcome(list<client_operation*>& server_op_list){

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
        cerr << "Something went horribly wrong in comm::receive_welcome()" << endl;
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
            for(auto ptr : server_op_list){
                if(op_name == ptr->get_name()){
                    ptr->make_available();
                }
            }
        }

        /**
            At this point, the client is informed about what operations the server offers.
        */


    }

    delete[] c_message;
}

int comm::send_message(const std::string& message){

	//potenzielle Verbesserung: string in stringstream umwandeln und mit buffern arbeiten

    unsigned int total = 0;
    int bytes_left = message.length(); //??
    int bytes_sent = 0;

    const char * c_message = new char[message.size()+1];
    c_message = message.c_str();

    while(total < message.length()){
        bytes_sent = send(this->sd, c_message+total, bytes_left, 0);
        if(-1 == bytes_sent ){
            cerr << "Error while sending a message to the server." << endl;
            return -1;
        }
        total += bytes_sent;
        bytes_left -= bytes_sent; //brauch ich das?
    }

    return total;
}

int comm::send_file(const std::string& filename){

	//TODO: throw exception when file could not be opened

	//CHECK AND PREPARE
	string filepath = dir_handler::make_absolute_path(filename);
	string raw_filename = dir_handler::filename_from_path(filepath);

	ifstream attachment_file(filepath, ios::in | ios::binary);

	if(!attachment_file.is_open()){
		cerr << "file to be sent could not be opened" << endl;
		return -1;
		//throw here
	}

	int total = 0;
	int bytes_sent = 0;
	int bytes_of_buffer_sent = 0;
	int padding_bytes = 0;

	attachment_file.seekg(0, attachment_file.end);
	int filesize = attachment_file.tellg();
	int bytes_left = filesize;
	attachment_file.seekg(0, attachment_file.beg);

	/**
	 * 	padding_bytes is the number of bytes at the end of the buffer
	 * 	that should be ignored when sending a not fully filled buffer,
	 * 	as it might appear when reading from a file when the buffer size is bigger
	 * 	than the bytes left to read from the file
	 */

	//SEND HEADER
	string header = raw_filename + "\n";
	header += to_string(filesize) + "\n";

	this->send_message(header);

	//RECEIVE CONFIRMATION
	string confirmation;
	this->receive_message(confirmation);
	if(confirmation != "GO"){
		cerr << "Server was not ready to receive the file" << endl;
		return -1;
		//TODO: throw here
	}

	//SEND FILE
	char * bbuffer = new char[MSG_BUF];
	memset(bbuffer, 0, MSG_BUF);

	while(total<filesize){ //Es sind noch nicht alle Bytes der Datei geschickt worden
		if(MSG_BUF > bytes_left){
			attachment_file.read(bbuffer, bytes_left);
			padding_bytes = MSG_BUF - bytes_left;
		}
		else{
			attachment_file.read(bbuffer, MSG_BUF);
		}

		bytes_of_buffer_sent = 0;
		while(bytes_of_buffer_sent+padding_bytes < MSG_BUF){ //Es ist noch nicht der gesamte Buffer geschickt worden
			bytes_sent = send(this->sd, bbuffer+bytes_of_buffer_sent, MSG_BUF-bytes_of_buffer_sent-padding_bytes, 0);
			if(-1 == bytes_sent ){
				cerr << "Error while sending a file to the server." << endl;
				return -1;
			}
			else{
				bytes_of_buffer_sent += bytes_sent;
			}
		}
		if(bytes_of_buffer_sent > MSG_BUF){
			cerr << "more bytes sent than intended, fatal error" << endl;
			return -1;
		}
		total += bytes_of_buffer_sent;
		bytes_left -= bytes_of_buffer_sent;
	}

	delete[] bbuffer;
	attachment_file.close();

	cout << "filesize: " << filesize << "; and number of bytes sent: " << total << endl;

	return total;
}

int comm::receive_message(std::string& buf_message){

	//TODO: buffered reading, below implementation might be problematic for really long messages

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
        buf_message = c_message;
    }
    delete[] c_message;
    return message_size;
}

int comm::receive_file(){

}
