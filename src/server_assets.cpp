#include "../headers/server_assets.h"

#include <dirent.h>

using namespace std;

server_assets::server_assets(){

}

server_assets::~server_assets(){

}

void server_assets::shutdown(){
	this->server_running = false;
}

void server_assets::login_user(const string& username){
	this->username = username;
	this->login_tries = 0;
}

void server_assets::logout_user(){
	this->username.clear();
}

bool server_assets::is_user_logged_in(){
	return !this->username.empty();
}

string server_assets::get_username(){
	return this->username;
}

bool server_assets::is_user_banned(const string& username){

	//TODO: implement

	return false;
}

bool server_assets::LDAP_login(const string& username, const string& password){

	//TODO: implement (copy code from login_operation)
	//TODO: excpetion when error occurs

	return true;
}

void server_assets::user_failed_login(){
	++this->login_tries;
}

unsigned int server_assets::get_login_tries(){
	return this->login_tries;
}

void server_assets::ban_user(const string& username){

	//TODO: implement
}

bool server_assets::mail_list_empty(){
    return this->mail_list.empty();
}

list<mail> server_assets::get_mail_list(){

    return this->mail_list;
}

void server_assets::create_mail_list(const string& username){

	//TODO: exceptions when errors

    if(!this->mail_list.empty()){
        this->mail_list.clear();
    }

    string userpath_string = my_handler.make_absolute_user_dir_path(username);
    //1. richtiges Verzeichnis öffnen
    if(userpath_string.empty()){
        return; //throw here
    }
    const char* userpath = new char[userpath_string.length()+1];
    userpath = userpath_string.c_str();

    struct dirent *direntp;
    DIR *dirp;

    if ((dirp = opendir(userpath)) == NULL) {
        cerr << "Failed to open directory: " << userpath << endl;
        return; //throw here
    }
    else{
        int mail_id = 1; //TODO: bessere Lösung als das...
        while ((direntp = readdir(dirp)) != NULL) {
            if(strncmp(direntp->d_name, ".", 1) == 0  ||
                strncmp(direntp->d_name, "..", 2) == 0){
                continue;
            }
            //2. Datei für Datei öffnen und Informationen aus Namen + Inhalt lesen
            string filename = direntp->d_name;
            string file_path = my_handler.make_absolute_user_dir_path(username) + filename;
            mail mail_obj = mail::make_list_mail_from_file(file_path, filename);
            mail_obj.set_receiver(username);
            mail_obj.set_mail_id(mail_id);
            this->mail_list.push_back(mail_obj);
            ++mail_id;
        }
    }
    while ((closedir(dirp) == -1) && (errno == EINTR));

}



string server_assets::mail_list_to_message(){

    stringstream return_stream;
    return_stream   << this->mail_list.size() << "\n";

    for(const mail& mail_obj : this->mail_list){
    	return_stream << "NEXT\n";
    	return_stream << mail_obj.get_mail_id() << "\n";
    	return_stream << mail_obj.get_sender() << "\n";
    	return_stream << mail_obj.get_subject() << "\n";
    	return_stream << mail_obj.get_attachment_count() << "\n"; //WTF?
        //return_stream << mail_obj.get_mail_id() << ". " << mail_obj.get_subject() << "\n";
        for( string attachment_name : mail_obj.get_attachment_names()){
            return_stream << attachment_name << "\n";
        }
    }

    return return_stream.str();
}

void server_assets::update_attachment(const std::string& attachment_path){
	//TODO: implement

	/**
	 * Der Dateiname des Attachments wird aktualisiert -> -1
	 * Falls der Counter im Dateiname auf null sinkt, wird die Datei stattdessen gelöscht
	 */

	//Mail pointer counter extrahieren

	//counter überprüfen

	// > 0 -> aktualisieren

	// = 0 -> löschen

}
