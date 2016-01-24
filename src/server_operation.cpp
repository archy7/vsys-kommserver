/*#include "../headers/mailserver.h"
#include "../headers/server_operation.h"
#include "../headers/dir_handler.h"
#include "../headers/mails.h"*/

#include "../headers/server_operation.h"

#define MAX_LOGIN 3

using namespace std;

server_operation::server_operation(int label, string name){
    this->label = label;
    this->name  = name;
}

server_operation::~server_operation(){

}
//warum undefined reference??

/*string server_operation::get_name(){
	return this->name;
}*/

void s_login_operation::execute(server_assets& my_assets, server_comm& my_comm, stringstream& message_stream){

	stringstream response_stream;

    string username;
    string password;

    getline(message_stream, username);
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
    getline(message_stream, password);
    /*getline(message_stream, last1);

    if(last1.compare(".")!=0){
        return "ERR(Protocol was violated)\n";
    }*/

    /*LDAP *ld;			// LDAP resource handle
    LDAPMessage *result, *e;	// LDAP result handle
    //BerElement *ber;		// array of attributes
    //char *attribute;
    //char **vals;

    //int i,rc=0;
    int rc=0;

    char *attribs[3];		// attribute array for search

    attribs[0]=strdup("uid");		// return uid and cn of entries
    attribs[1]=strdup("cn");
    attribs[2]=NULL;		// array must be NULL terminated

    if ((ld=ldap_init(LDAP_HOST, LDAP_PORT)) == NULL)
    {
      perror("ldap_init failed");
      return "ERR(Could not reach LDAP Server)\n";
    }

    //printf("connected to LDAP server %s on port %d\n",LDAP_HOST,LDAP_PORT);

    rc = ldap_simple_bind_s(ld,ANON_USER,ANON_PW);

    if (rc != LDAP_SUCCESS)
    {
      fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
      return "ERR(LDAP Error: anon bind failed)\n";
    }

    string filter = "(uid=";
    filter += username;
    filter += ")";
    const char* c_user = new char[filter.length()+1];
    c_user = filter.c_str();

    rc = ldap_search_s(ld, SEARCHBASE, SCOPE, c_user, attribs, 0, &result);

    if (rc != LDAP_SUCCESS)
    {
      fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
      return "ERR(LDAP Error: search failed)\n";
    }

    int entry_count;
    //printf("Total results: %d\n", ldap_count_entries(ld, result));
    if((entry_count = ldap_count_entries(ld, result)) != 1){
        if(0 == entry_count){
            return "ERR(Specified uid not found)\n";
        }
        else if(entry_count > 1){
            return "ERR(Critical error: More than one user with specified uid exists)\n";
        }

    }

    e = ldap_first_entry(ld, result);

    const char* c_pass = new char[password.length()+1];
    c_pass = password.c_str();

    rc = ldap_simple_bind_s(ld,ldap_get_dn(ld,e),c_pass);

    if (rc != LDAP_SUCCESS)
    {
      fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
      return "ERR(LDAP Error: authenication failed)\n";
    }

    // free memory used for result
    ldap_msgfree(result);
    free(attribs[0]);
    free(attribs[1]);
    //printf("LDAP search suceeded\n");

    ldap_unbind(ld);

    this_handler.set_username(username);

    return "OK\nWelcome, " + username + "!\n";*/

    //CHECK 1
    if(my_assets.is_user_banned(username)){
    	response_stream << "ERR\nUser is still banned.\n";
    	//response_stream << TIME UNTIL NO LONGER BANNED
    }
    else
    //CHECK 2
	if(my_assets.LDAP_login(username, password) == false){
		my_assets.user_failed_login();

		response_stream << "ERR\nFailed to login. Credentials were wrong.\n";

		if(MAX_LOGIN == my_assets.get_login_tries()){
			my_assets.ban_user(username);
			response_stream << "Too many failed Login-Attempts (" << MAX_LOGIN << "). You are now banned for 30 minutes.\n";
		}
    }
    else{
    	my_assets.login_user(username);
    	response_stream << "OK\nWelcome, " + username + "!\n";
    }

    string response = response_stream.str();

    my_comm.send_message(response);
}

void s_send_operation::execute(server_assets& my_assets, server_comm& my_comm, stringstream& message_stream){

	stringstream response_stream;


    string sender;
    string receivers;
    vector<string> receiver_vec;
    string subject;
    string line;

    string attachment_count;

    string content;

    string last4;

    //Sender, Receiver and Subject are all delimited by \n
    getline(message_stream, sender);
    getline(message_stream, receivers);
    stringstream receiverstream(receivers);
    string receiver;
    while(getline(receiverstream, receiver, ';')){
        receiver_vec.push_back(receiver);
    }

    getline(message_stream, subject);

    //Der Rest ist Inhalt + \n.\n
    bool reading_content = true;
    while(!message_stream.eof()){
        if(reading_content){
            getline(message_stream, line);
            if(line == "."){
                reading_content = false;
                continue;
            }
            content += line;
            content += "\n";

        }
        else{
            getline(message_stream, attachment_count);
            break;
        }

    }

    /*if(last4.compare("\n.\n\n")!=0){
        return "ERR\n(Protocol was violated)";
    }*/

    /**
        komtrollen: SIND ZU LÖSCHEN
    cout << "Sender: " << sender << endl;
    cout << "Receivers: "; for (auto rec : receiver_vec){ cout << rec << "; ";} cout << endl;
    cout << "Subject: " << subject << endl;
    cout << "Content: " << content << endl;
    cout << "Attachment count: " << attachment_count << endl;
    */

    int i_attachment_count = stoi(attachment_count);

    mail new_mail = mail::make_new_mail(sender, receiver, subject, content, i_attachment_count); //missing: attachment_paths, receiver

    //list<string> filepaths;

    for(int i=0; i<i_attachment_count; ++i){

        /*string prefix = to_string(receiver_vec.size()) + ":";
        string filename = prefix + filename;
        string abs_att_path = my_assets.my_handler.make_absolute_attachment_path(filename);*/
    	string dirpath = my_assets.my_handler.make_absolute_attachment_dir_path();

        my_comm.send_message("GO");
        //cout << "receiving file " << filename << " with size: " << filesize << endl;
        int total = my_comm.receive_file(dirpath, receiver_vec.size(), new_mail);
        //cout << "received file: " << filename << " with size: " << total << endl;;

        //cout << "absolute path to file: " << abs_filename << endl;
        //filepaths.push_back(abs_att_path);
    }

    bool save_succesful = true;
    for (auto rec : receiver_vec){
        if(my_assets.my_handler.user_dir_exists(rec)==false){
            cout << "Could not find user directory, therefore creating one" << endl;
            my_assets.my_handler.make_user_dir(rec);
        }

        //save it
        string userpath = my_assets.my_handler.make_absolute_user_dir_path(rec);
        if(new_mail.save_to_file(userpath) == false){
            save_succesful = false;
        }
    }

    if(!save_succesful){
    	response_stream <<  "ERR\nCould not save an attachment.\n";
    }
    else{
    	response_stream << "OK\n";
    }

    my_comm.send_message(response_stream.str());

}

void s_read_operation::execute(server_assets& my_assets, server_comm& my_comm, stringstream& message_stream){

	stringstream response_stream;

    string receiver;
    string mail_id;

    getline(message_stream, receiver);
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
    getline(message_stream, mail_id);


    if(my_assets.my_handler.user_dir_exists(receiver)==false){
        response_stream <<  "ERR\nUser directory does not exist. The user has never received a mail.\n";
        my_comm.send_message(response_stream.str());
        return;
    }

    if(my_assets.mail_list_empty()){
    	my_assets.create_mail_list(receiver);
    }

    int i_maild_id = stoi(mail_id);

    for(const mail& mail_obj: my_assets.get_mail_list()){
        if(mail_obj.get_mail_id() == i_maild_id){
            response_stream << "OK\n";
            response_stream <<  mail_obj.get_content();
            my_comm.send_message(response_stream.str());
            return;
        }
    }

    response_stream << "ERR\nMail does not exist.\n";
    my_comm.send_message(response_stream.str());

}

void s_list_operation::execute(server_assets& my_assets, server_comm& my_comm, stringstream& message_stream){

	stringstream response_stream;

    string receiver;
    string last1;

    getline(message_stream, receiver);
    getline(message_stream, last1);


    if(my_assets.my_handler.user_dir_exists(receiver)==false){
        response_stream <<  "ERR\nUser directory does not exist. The user has never received an email.\n";
    }

    my_assets.create_mail_list(receiver);

    response_stream << "OK\n";

    response_stream << my_assets.mail_list_to_message();

    my_comm.send_message(response_stream.str());

}

void s_delete_operation::execute(server_assets& my_assets, server_comm& my_comm, stringstream& message_stream){

	stringstream response_stream;

    string receiver;
    string mail_id;

    getline(message_stream, receiver);
    getline(message_stream, mail_id);

    if(my_assets.my_handler.user_dir_exists(receiver)==false){
    	response_stream <<  "ERR\nUser directory does not exist. The user has never received an email.\n";
        my_comm.send_message(response_stream.str());
        return;
    }

    if(my_assets.mail_list_empty()){
    	my_assets.create_mail_list(receiver);
    }


    bool delete_succesful = false;
    for(const mail& mail_obj: my_assets.get_mail_list()){

        if(mail_obj.get_mail_id() == stoi(mail_id)){
            string userpath = my_assets.my_handler.make_absolute_user_dir_path(receiver);
            delete_succesful = mail_obj.delete_file(userpath);
            if(delete_succesful){

            	for(const string& attachment_path : mail_obj.get_attachment_paths()){
            		my_assets.my_handler.update_attachment(attachment_path);
            	}
            	my_assets.create_mail_list(receiver);
                break;
            }
        }
    }

    if(delete_succesful){
    	response_stream <<  "OK\n";
    }
    else{
    	response_stream <<  "ERR\nSpecified mail not found.\n";
    }

    my_comm.send_message(response_stream.str());
}

void s_quit_operation::execute(server_assets& my_assets, server_comm& my_comm, stringstream& message_stream){

	my_assets.logout_user();
	my_assets.shutdown();

	my_comm.send_message("GOOD BYE!\n");
}
