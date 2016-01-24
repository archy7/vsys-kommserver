#include "../headers/mails.h"

using namespace std;

mail::mail(){

    this->mail_id  = 0;
    this->sender   = "";
    this->receiver = "";
    this->subject  = "";
    this->content  = "";
    this->filename = "";
    this->attachment_count = 0;
}

mail::~mail(){

}

void mail::set_receiver(string receiver){

    this->receiver = receiver;
}

bool mail::has_content(){
	return !this->content.empty();
}

bool mail::has_attachment(unsigned int id){

	//TODO: throw exception when id < 1

	//return id<=this->attachment_count;
	return id<=this->attachment_paths.size();
}

bool mail::has_attachments(){
	return this->attachment_count > 0;
}

/*int mail::get_attachment_count(){
	return this->attachment_count;
	//return this->attachment_paths.size();
}*/

unsigned int mail::get_attachment_count() const{
	return this->attachment_count;
}

string mail::get_content() const{
	return this->content;
}

void mail::set_content(string new_content){

	this->content = new_content;
}

void mail::set_mail_id(int mail_id){

    this->mail_id = mail_id;
}

void mail::add_attachment_path(string attachment_path){
	this->attachment_paths.push_back(attachment_path);
}

int mail::get_mail_id() const{

    return this->mail_id;
}

string mail::get_subject() const{

    return this->subject;
}

string mail::get_sender() const{

	return this->sender;
}

string mail::create_file_name(string user_path){

    auto t = time(nullptr);
    auto now = localtime(&t);

    auto randchar = [](){

        const char charset[] = "123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const size_t max_index = (sizeof(charset)-1);
        return charset[ rand() % max_index];

    };

    string random_string(10, 0);
    generate_n(random_string.begin(), 10, randchar);

    stringstream date_time;
    date_time << (now->tm_year +1900) << "-"              //put_time(&tm, "%Y-%m-%d-%H:%M:%S")
            << (now->tm_mon +1) << "-"
            << (now->tm_mday) << "-"
            << (now->tm_hour) << ":"
            << (now->tm_min) << ":"
            << (now->tm_sec);

    string date_string = date_time.str();

    long thread_id = pthread_self();
    string thread_id_string = to_string(thread_id);

    string filename(user_path);

    filename += date_string;
    filename += ".";
    filename += thread_id_string;
    filename += ".";
    filename += random_string;
    filename += ".";
    filename += this->sender;

    /*stringstream filename;
    filename<< user_path
            << (now->tm_year +1900) << "-"              //put_time(&tm, "%Y-%m-%d-%H:%M:%S")
            << (now->tm_mon +1) << "-"
            << (now->tm_mday) << "-"
            << (now->tm_hour) << ":"
            << (now->tm_min) << ":"
            << (now->tm_sec)
            << "." << random_string << "." << this->sender;*/

    return filename;
}

mail mail::make_new_mail(string sender, string receiver, string subject, string content, int attachment_count){

    mail this_mail;
    this_mail.sender = sender;
    this_mail.receiver = receiver;
    this_mail.subject = subject;
    this_mail.content = content;
    this_mail.attachment_count = attachment_count;
    //not set: attachment_paths

    return this_mail;
}

mail mail::make_list_mail_from_file(string filepath, string file_name){

    mail this_mail;
    this_mail.filename = file_name; //could maybe use that. leaving it for now
    /**
    this_mail.sender = sender;      //In filename ->to be set here
    this_mail.receiver = receiver;  //name of dir -> set outside this method
    this_mail.subject = subject;    //the first line
    this_mail.attachment_count;     //the second line
    this_mail.attachments;          //the next 'attachment_count' lines
    this_mail.content = content;    //rest of content -> to be set here
    */

    //Sender
    this_mail.sender = file_name.substr(file_name.rfind('.')+1);

    ifstream mail_file_stream;
    mail_file_stream.open(filepath);

    //Subject
    getline(mail_file_stream, this_mail.subject);

    //Attachment_count
    string attachment_count_str;
    getline(mail_file_stream, attachment_count_str);
    this_mail.attachment_count = stoi(attachment_count_str);

    //Attachments
    string attachment;
    for(unsigned int i = 0; i< this_mail.attachment_count; ++i){
        getline(mail_file_stream, attachment);
        this_mail.attachment_paths.push_back(attachment);
        attachment.clear();
    }


    //Content not needed for list mail
    /*while(!mail_file_stream.eof()){
        string content_line;
        //mail_file_stream >> this_mail.content;
        getline(mail_file_stream, content_line);
        this_mail.content += content_line;
        this_mail.content += "\n";
    }*/


    mail_file_stream.close();
    return this_mail;
}

bool mail::save_to_file(string user_path){

    if(this->filename.empty()){
        ofstream new_mail_file;
        string filename = this->create_file_name(user_path);
        new_mail_file.open(filename);

        if(!new_mail_file.is_open()){
            cerr << "CRITICAL ERROR WHILE CREATING NEW MAIL FILE" << endl;
            cerr << "file could not be opened: " << filename << endl;
            return false;
        }

        new_mail_file << this->subject << "\n"
                      << to_string(this->attachment_count) << "\n";

        if(this->attachment_count > 0){
            for( string filepath : this->attachment_paths){
                new_mail_file << filepath << "\n";
            }
        }

        new_mail_file << this->content << "\n";


        new_mail_file.close();
        return true;
    }

    return false;
}

list<string> mail::get_attachment_paths() const{

	return this->attachment_paths;
}

list<string> mail::get_attachment_names() const{

	list<string> attachment_names;

	for(string attachment_path : this->attachment_paths){
		string attachment_name = attachment_path.substr(attachment_path.rfind(':')+1);
		attachment_names.push_back(attachment_name);
	}
	return attachment_names;
}

string mail::to_message() const{

    int i = 1;

    stringstream return_stream;
    return_stream   << "Sender: " << this->sender << "\n"
                    << "Receiver: " << this->receiver << "\n"
                    << "Subject: " << this->subject << "\n"
                    << "Number of attachments: " << this->attachment_count << "\n";

    for( string attachment_name : this->attachment_paths){
        return_stream << "Attachment #" << i << ": " << attachment_name << "\n";
    }

    return_stream   << "Content: " << "\n" << this->content;

    return return_stream.str();
}

bool mail::delete_file(string user_path) const{

    if(!this->filename.empty()){
        string filename = user_path + this->filename;
        if(remove(filename.c_str()) ==0){
            return true;
        }
    }

    return false;
}
