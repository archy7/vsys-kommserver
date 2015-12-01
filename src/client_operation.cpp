#include "../headers/mailclient.h"
#include "../headers/client_operation.h"

#include "../headers/communication.h"
#include "../headers/UI.h"

using namespace std;

client_operation::client_operation(int label, string name){
    this->label = label;
    this->name  = name;
}

client_operation::~client_operation(){

}

string client_operation::get_name(){
	return this->name;
}

void client_operation::make_available(){
	this->available = true;
}

void login_operation::execute(client_assets& my_assets, comm& my_comm){

	//CHECK
	//TODO: Was ist wenn der User schon eingeloggt ist?

	//PREPARE
    string message = "LOGIN\n";
    string username = my_assets.my_UI.prompt_username() + "\n";
    message += username;
    message += my_assets.my_UI.prompt_password(true) + "\n";
    message += ".\n";

    //SEND
    my_comm.send_message(message);

    //RECEIVE
    string answer;
    my_comm.receive_message(answer);

    //HANDLE
    stringstream answerstream(answer);
    string success;
    getline(answerstream, success, '\n');
    if(0 == success.compare("OK")){
    	my_assets.set_username(username);
    	cout << "Welcome, " << username << "!" << endl;
    }
    else{
    	cout << "Login unsuccesful. You were not logged in and must try again." << endl;
    }

}

void send_operation::execute(client_assets& my_assets, comm& my_comm){

	//CHECK
    if(false ==my_assets.user_logged_in()){
        cout << "You need to login first." << endl;
        return;
    }

    //PREPARE
    string message = "SEND\n";

    message += my_assets.get_username()+ "\n";
    message += my_assets.my_UI.prompt_receiver_list() + "\n";
    message += my_assets.my_UI.prompt_subject() + "\n";
    message += my_assets.my_UI.prompt_content() + "\n";
    message += ".\n";

    list<string> filenames = my_assets.my_UI.prompt_attachments();
    int attachment_count = filenames.size();
    message += to_string(attachment_count) + "\n";

    /*if(attachment_count > 0){
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

    }*/

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

    Die binären Daten der Datei werden in einem separaten send verschickt.

    */

    //cout << message;
    my_comm.send_message(message);


    //int i = 0;
	for( auto filename : filenames){

		string confirmation;
		my_comm.receive_message(confirmation);
		cout << confirmation << endl;
		if(confirmation=="GO"){
			//const char* c_path = new char[filepath.length()+1];
			//c_path = filepath.c_str();

			//FILE* fp = fopen(c_path, "rb");
			//int in_fd = fileNONE(fp);
			//unsigned char buffer[1024];
			//sendfile(this_client->sd, in_fd, NULL, 1024);
			//cout << "sending file NONEw: " << filepath << " with size: " << filesizes[i] << endl;
			my_comm.send_file(filename);
			//receive an answer to the data transfer

		}
		else{
			cerr << "Error during file transmission communication" << endl;
			//TODO: throw here
		}


	}

    string answer;
    my_comm.receive_message(answer);
    cout << answer;


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

void list_operation::execute(client_assets& my_assets, comm& my_comm){

	//CHECK
    if(false ==my_assets.user_logged_in()){
        cout << "You need to login first." << endl;
        return;
    }

    //PREPARE
    string message = "LIST\n";
    message += my_assets.get_username()+ "\n";

    //SEND
    my_comm.send_message(message);

    //RECEIVE
    string answer;
    my_comm.receive_message(answer);

    //HANDLE
    //TODO: build local maillist
    my_assets.clear_mail_list();
    my_assets.build_mail_list(answer);

}

void read_operation::execute(client_assets& my_assets, comm& my_comm){

	//CHECK
    if(false ==my_assets.user_logged_in()){
        cout << "You need to login first." << endl;
        return;
    }
    //CHECK 2
    if(my_assets.mail_list_empty()){
    	cout << "You need to perform the LIST command first in order to read specific mails." << endl;
    	return;
    }

    unsigned int mail_id = my_assets.my_UI.prompt_mail_id();
    //CHECK 3
    if(!my_assets.mail_exists(mail_id)){
    	cout 	<< "The desired mail does not exist." << endl
    			<< "If you are expecting a mail, try to perform the LIST command again to refresh the mail list." << endl;
    	return;
    }

    //CHECK 4
    mail this_mail = my_assets.find_mail_by_id(mail_id);
    if(this_mail.has_content()){
    	my_assets.my_UI.print_mail(this_mail);
    	return;
    }

    //PREPARE
	string message = "READ\n";
	message += my_assets.get_username()+ "\n";

    /*mail& this_mail = my_assets.find_mail_by_id(mail_id);
    int att_dl = 0;

    if(this_mail.has_attachments()){
    	att_dl = my_assets.my_UI.prompt_download_attachment();
    	if(att_dl != 0){
    		if(att_dl == -1){
    			message += att_dl + "\n";
    		}
    		else{
    			unsigned int attachment_id = (unsigned int) att_dl;
    			if(false == this_mail.has_attachment(attachment_id)){
    				cout 	<< "Specified attachment does not exist." << endl
    						<< "NONE attachment will be downloaded." << endl;
    				message += "NONE\n";
    			}
    			else{
    				cout 	<< "Downloading specified attachment(s)." << endl;
    				message += att_dl + "\n";
    			}
    		}
    	}
    }
    else{
    	message += "0\n";
    }*/


    message += to_string(mail_id) + "\n";

    cout << "TEST: MESSAGE:" << endl;
    cout << message;

    //SEND
    my_comm.send_message(message);

    //RECEIVE
    string answer;
    my_comm.receive_message(answer);

    //HANDLE
    this_mail.set_content(answer);
    my_assets.my_UI.print_mail(this_mail);

}

void delete_operation::execute(client_assets& my_assets, comm& my_comm){

	//CHECK
    if(false ==my_assets.user_logged_in()){
        cout << "You need to login first." << endl;
        return;
    }
    //CHECK 2
    if(my_assets.mail_list_empty()){
    	cout << "You need to perform the LIST command first in order to read specific mails." << endl;
    	return;
    }

    unsigned int mail_id = my_assets.my_UI.prompt_mail_id();
    //CHECK 3
    if(!my_assets.mail_exists(mail_id)){
    	cout 	<< "The desired mail does not exist." << endl;
    	return;
    }

    //PREPARE
    string message = "DEL\n";
	message += my_assets.get_username()+ "\n";
	message += to_string(mail_id);

    //SEND
    my_comm.send_message(message);

    //RECEIVE
    string answer;
    my_comm.receive_message(answer);

    //HANDLE
    cout << answer;

}

void quit_operation::execute(client_assets& my_assets, comm& my_comm){

	//CHECK
    if(false ==my_assets.user_logged_in()){
    	my_assets.shutdown();
        cout << "Good Bye." << endl;
        return;
    }

    //PREPARE
    string message = "QUIT\n";
	message += my_assets.get_username()+ "\n";

    //SEND
    my_comm.send_message(message);

    //RECEIVE
    string answer;
    my_comm.receive_message(answer);

    //HANDLE
    cout << answer;
}
