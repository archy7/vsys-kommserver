#include "../headers/mailserver.h"
#include "../headers/server_operation.h"
#include "../headers/dir_handler.h"
#include "../headers/mails.h"

using namespace std;

server_operation::server_operation(int label, string name){
    this->label = label;
    this->name  = name;
}

server_operation::~server_operation(){

}

string login_operation::execute(mailserver* this_server, int stream_sd, stringstream& message_stream, dir_handler& this_handler){

    string username;
    string password;
    string last1;

    /**
     * KOMMENTAR OF DOOM
     */


    getline(message_stream, username);
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
    getline(message_stream, password);
    getline(message_stream, last1);

    if(last1.compare(".")!=0){
        return "ERR(Protocol was violated)\n";
    }

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

    this_handler.set_username(username);
    return "OK\nWelcome, " + username + "!\n";

}

string send_operation::execute(mailserver* this_server, int stream_sd, stringstream& message_stream, dir_handler& this_handler){

    if(false == this_handler.user_logged_in()){

        return "ERR\n(No user is logged in)";
    }

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
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
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


    //Die letzten 3 Zeichen herausziehen und auf Korrektheit überprüfen \n.\n
    //last4 = content.substr(content.length()-4);

    /*if(last4.compare("\n.\n\n")!=0){
        return "ERR\n(Protocol was violated)";
    }*/

    //content = content.substr(0,content.rfind("\n.\n"));


    /**
        komtrollen: SIND ZU LÖSCHEN
    cout << "Sender: " << sender << endl;
    cout << "Receivers: "; for (auto rec : receiver_vec){ cout << rec << "; ";} cout << endl;
    cout << "Subject: " << subject << endl;
    cout << "Content: " << content << endl;
    cout << "Attachment count: " << attachment_count << endl;
    */



    int i_attachment_count = stoi(attachment_count);


    vector<string> filepaths;
    vector<int> filesizes;

    if(i_attachment_count > 0){

        for(int i=0; i<i_attachment_count; ++i){


            string filename;
            string filesize;
            getline(message_stream, filename);
            getline(message_stream, filesize);

            string prefix = to_string(receiver_vec.size()) + ":";
            filename = prefix + filename;
            string abs_filename = this_handler.make_absolute_attachment_path(filename);

            this_server->send_all(stream_sd, "GO");
            cout << "receiving file " << filename << " with size: " << filesize << endl;
            int total = this_server->receive_file(stream_sd, abs_filename, stoi(filesize));
            cout << "received file: " << filename << " with size: " << total << endl;;



            //cout << "absolute path to file: " << abs_filename << endl;
            filepaths.push_back(abs_filename);
            filesizes.push_back(stoi(filesize));
        }
    }

    //VERIFY PROTOCOL
    getline(message_stream, line);
    if(line != "."){
        return "ERR\n(Protocol was violated)";
    }




    bool save_succesful = true;
    for (auto rec : receiver_vec){
        if(this_handler.user_dir_exists(rec)==false){
            cout << "Could not find user directory, therefore creating one" << endl;
            this_handler.make_user_dir(rec);
        }

        mail new_mail = mail::make_new_mail(sender, rec, subject, content, i_attachment_count, filepaths, filesizes);

        //save it
        string userpath = this_handler.make_absolute_user_path(rec);
        if(new_mail.save_to_file(userpath) == false){
            save_succesful = false;
        }
    }

    return (save_succesful) ? "OK\n" : "ERR(could not save mails or attachments)\n";
}

string read_operation::execute(mailserver* this_server, int stream_sd, stringstream& message_stream, dir_handler& this_handler){

    if(false == this_handler.user_logged_in()){
        return "ERR\n(No user is logged in)";
    }

    string receiver;
    string mail_id;
    string last1;

    getline(message_stream, receiver);
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
    getline(message_stream, mail_id);
    getline(message_stream, last1);

    if(last1.compare(".")!=0){
        return "ERR(Protocol was violated)\n";
    }

    if(this_handler.user_dir_exists(receiver)==false){
        return "ERR(User does not exist)\n";
    }

    if(this_handler.mail_list_is_empty()){
        this_handler.create_mail_list(receiver);
    }

    for(const mail& mail_obj: this_handler.get_mail_list()){
        if(mail_obj.get_mail_id() == stoi(mail_id)){
            return "OK\n" + mail_obj.to_message();
        }
    }

    return "ERR\n(Message not found)";

}

string list_operation::execute(mailserver* this_server, int stream_sd, stringstream& message_stream, dir_handler& this_handler){

    if(false == this_handler.user_logged_in()){
        return "ERR\n(No user is logged in)";
    }

    string receiver;
    string last1;

    getline(message_stream, receiver);
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
    getline(message_stream, last1);

    if(last1.compare(".")!=0){
        return "ERR\n(Protocol was violated)";
    }

    if(this_handler.user_dir_exists(receiver)==false){
        return "ERR\n(User does not exist)";
    }

    this_handler.create_mail_list(receiver);

    return this_handler.list_to_message();

}

string delete_operation::execute(mailserver* this_server, int stream_sd, stringstream& message_stream, dir_handler& this_handler){

    if(false == this_handler.user_logged_in()){
        return "ERR\n(No user is logged in)";
    }

    string receiver;
    string mail_id;
    string last1;

    getline(message_stream, receiver);
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
    getline(message_stream, mail_id);
    getline(message_stream, last1);

    if(last1.compare(".")!=0){
        return "ERR\n(Protocol was violated)";
    }

    if(this_handler.user_dir_exists(receiver)==false){
        return "ERR\n(User does not exist)";
    }

    if(this_handler.mail_list_is_empty()){
        this_handler.create_mail_list(receiver);
    }


    bool delete_succesful = false;
    for(const mail& mail_obj: this_handler.get_mail_list()){
        if(mail_obj.get_mail_id() == stoi(mail_id)){
            string userpath = this_handler.make_absolute_user_path(receiver);
            delete_succesful = mail_obj.delete_file(userpath);
            if(delete_succesful){
                //this_handler.update_attachment(attachment_path);
                this_handler.create_mail_list(receiver);
                break;
            }
        }
    }

    return (delete_succesful) ? "OK\n" : "ERR\n";

}

string quit_operation::execute(mailserver* this_server, int stream_sd, stringstream& message_stream, dir_handler& this_handler){

    /*if(false == this_handler.user_logged_in()){
        return "ERR\n(No user is logged in)";
    }*/

    return "GOOD BYE!\n";
}
