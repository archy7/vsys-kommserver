#include "../headers/mails.h"

using namespace std;

mail::mail(){

    this->mail_id  = 0;
    this->sender   = "";
    this->receiver = "";
    this->subject  = "";
    this->content  = "";
    this->filename = "";
}

mail::~mail(){

}

void mail::set_receiver(string receiver){

    this->receiver = receiver;
}

void mail::set_mail_id(int mail_id){

    this->mail_id = mail_id;
}

int mail::get_mail_id() const{

    return this->mail_id;
}

std::string mail::get_subject() const{

    return this->subject;
}

string mail::create_file_name(string user_path){

    auto t = time(nullptr);
    auto now = localtime(&t);

    auto randchar = [](){

        const char charset[] = "123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const size_t max_index = (sizeof(charset)-1);
        return charset[ rand() % max_index];

    };

    string random_string(15, 0);
    generate_n(random_string.begin(), 15, randchar);

    stringstream date_time;
    date_time << (now->tm_year +1900) << "-"              //put_time(&tm, "%Y-%m-%d-%H:%M:%S")
            << (now->tm_mon +1) << "-"
            << (now->tm_mday) << "-"
            << (now->tm_hour) << ":"
            << (now->tm_min) << ":"
            << (now->tm_sec);

    string date_string = date_time.str();

    string filename(user_path);
    /*filename += (now->tm_year +1900);
    filename += "-";
    filename += (now->tm_mon +1);
    filename += "-";
    filename += (now->tm_mday);
    filename += "-";
    filename += (now->tm_hour);
    filename += ":";
    filename += (now->tm_min);
    filename += ":";
    filename += (now->tm_sec);*/
    filename += date_string;
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

mail mail::make_new_mail(string sender, string receiver, string subject, string content){

    mail this_mail;
    this_mail.sender = sender;
    this_mail.receiver = receiver;
    this_mail.subject = subject;
    this_mail.content = content;

    return this_mail;
}

mail mail::make_mail_from_file(string filepath, string mail_name){

    mail this_mail;
    this_mail.filename = mail_name; //whole filename -> not set in this method
    /**
    this_mail.sender = sender;      //In filename ->to be set here
    this_mail.receiver = receiver;  //name of dir -> set outside this method
    this_mail.subject = subject;    //first line of content ->to be set here
    this_mail.content = content;    //rest of content -> to be set here
    */

    this_mail.sender = mail_name.substr(mail_name.rfind('.')+1);

    ifstream mail_file_stream;
    mail_file_stream.open(filepath);

    getline(mail_file_stream, this_mail.subject);
    while(!mail_file_stream.eof()){
        string content_line;
        //mail_file_stream >> this_mail.content;
        getline(mail_file_stream, content_line);
        this_mail.content += content_line;
        this_mail.content += "\n";
    }


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
                      << this->content << "\n";

        new_mail_file.close();
        return true;
    }

    return false;
}

string mail::to_message() const{

    stringstream return_stream;
    return_stream   << "Sender: " << this->sender << "\n"
                    << "Receiver: " << this->receiver << "\n"
                    << "Subject: " << this->subject << "\n"
                    << "Content: " << "\n" << this->content << "\n";

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
