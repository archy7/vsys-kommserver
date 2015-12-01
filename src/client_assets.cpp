#include "../headers/client_assets.h"

using namespace std;

client_assets::client_assets(){
	this->my_handler = dir_handler::make_client_handler();
}

client_assets::~client_assets(){

}

void client_assets::initialize(){

}

bool client_assets::user_logged_in(){
    return !this->username.empty();
}

void client_assets::set_username(std::string username){
	this->username = username;
}

string client_assets::get_username(){
	return this->username;
}

bool client_assets::mail_list_empty(){
	return this->mail_list.empty();
}

void client_assets::build_mail_list(std::string mailstring){
	//TODO: IMPLEMENT

}

void client_assets::clear_mail_list(){
	this->mail_list.clear();
}

bool client_assets::mail_exists(unsigned int id){

	//TODO: throw exception when mail_list is empty

	for( const mail& this_mail : this->mail_list){
		if(this_mail.get_mail_id() == id){
			return true;
		}
	}

	return false;
}

mail client_assets::find_mail_by_id(unsigned int id){

	//TODO: throw exception when mail_list is empty
	mail return_mail;

	for( mail this_mail : this->mail_list){
		if(this_mail.get_mail_id() == id){
			return_mail = this_mail;
		}
	}

	return return_mail;
}

void client_assets::shutdown(){
	this->client_running = false;
}
