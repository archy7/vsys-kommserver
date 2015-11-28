#include "../headers/mailserver.h"
#include "../headers/server_operation.h"
#include "../headers/dir_handler.h"


using namespace std;

dir_handler::dir_handler(){

}

dir_handler::~dir_handler(){

}

dir_handler dir_handler::make_dir_handler(string path){

    dir_handler this_handler;
    if(dir_exists(path)){
        this_handler.base_path = make_absolute_base_path(path);
    }
    else{
        this_handler.base_path = "";
        cerr << "Something went horribly wrong when making a dir_handler" << endl;
    }

    return this_handler;
}

void dir_handler::set_username(std::string username){
    this->username = username;
}

std::string dir_handler::get_username(){
    return this->username;
}

bool dir_handler::user_logged_in(){
    return !this->username.empty();
}

bool dir_handler::mail_list_is_empty(){

    return this->mail_list.empty();
}

list<mail> dir_handler::get_mail_list(){

    return this->mail_list;
}

int dir_handler::get_mail_list_length() const{

    return this->mail_list.size();
}

bool dir_handler::create_mail_list(string username){

    if(!this->mail_list.empty()){
        this->mail_list.clear();
    }

    //PSEUDO
    string userpath_string = this->make_absolute_user_path(username);
    //1. richtiges Verzeichnis öffnen
    if(userpath_string.empty()){
        return false;
    }
    const char* userpath = new char[userpath_string.length()+1];
    userpath = userpath_string.c_str();

    struct dirent *direntp;
    DIR *dirp;

    if ((dirp = opendir(userpath)) == NULL) {
        cerr << "Failed to open directory: \"" << userpath << "\"" << endl;
        return false;
    }
    else{
        int mail_id = 1;
        while ((direntp = readdir(dirp)) != NULL) {
            if(strncmp(direntp->d_name, ".", 1) == 0  ||
                strncmp(direntp->d_name, "..", 2) == 0){
                continue;
            }
            //2. Datei für Datei öffnen und Informationen aus Namen + Inhalt lesen
            //Die Ordnung des Systems akzeptieren (auf oder ab)
            string filename = direntp->d_name;
            string file_path = this->make_absolute_user_path(username);
            file_path += filename;
            mail mail_obj = mail::make_mail_from_file(file_path, filename);
            mail_obj.set_receiver(username);
            mail_obj.set_mail_id(mail_id);
            this->mail_list.push_back(mail_obj);
            ++mail_id;
        }
    }
    while ((closedir(dirp) == -1) && (errno == EINTR));

    //fertig
    //delete[] userpath;
    return true;
}

string dir_handler::list_to_message() const{

    stringstream return_stream;
    return_stream   << this->get_mail_list_length() << "\n";

    for(const mail& mail_obj : this->mail_list){
        return_stream << mail_obj.get_mail_id() << ". " << mail_obj.get_subject() << "\n";
        for( string attachment_name : mail_obj.get_attachment_names()){
            return_stream << "\t" << attachment_name << "\n";
        }
    }

    return return_stream.str();
}

string dir_handler::create_attachment_name(string filename){

    string new_filename;

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
    date_time << (now->tm_year +1900) << "-"
            << (now->tm_mon +1) << "-"
            << (now->tm_mday) << "-"
            << (now->tm_hour) << ":"
            << (now->tm_min) << ":"
            << (now->tm_sec);

    string date_string = date_time.str();

    int thread_id = pthread_self();
    string thread_id_string = to_string(thread_id);

    new_filename += date_string;
    new_filename += ".";
    new_filename += thread_id_string;
    new_filename += "_";
    new_filename += random_string;
    new_filename += ".";
    new_filename += filename;

    return new_filename;
}

string dir_handler::make_absolute_attachment_path(string filename){

    string full_filename = this->create_attachment_name(filename);
    string attachment_path = this->base_path;
    attachment_path += "/files/";
    attachment_path += full_filename;

    return attachment_path;
}

void dir_handler::update_attachment(std::string attachment_path){

}

string dir_handler::make_absolute_base_path(string path){

    const char* path_chars = new char[path.length()+1];
    path_chars = path.c_str();

    char *mail_path = canonicalize_file_name(path_chars);
    //delete[] path_chars;

    if(NULL!=mail_path){
        string base_path(mail_path);
        //cout << "base path created: " << base_path << endl;
        //delete[] path_chars;
        return base_path;
    }
    else{
        //delete[] path_chars;
        return "";
    }
}

string dir_handler::make_absolute_user_path(string username){

    string user_path = this->base_path;
    user_path += "/";
    user_path += username;
    user_path += "/";

    //cout << "user path created: " << user_path << endl;

    return user_path;
}

bool dir_handler::dir_exists(string path){

    string mail_path_string = make_absolute_base_path(path);
    if(mail_path_string.empty()){
        return false;
    }
    const char* mail_path = new char[mail_path_string.length()+1];
    mail_path = mail_path_string.c_str();


    bool mail_dir_exists = false;
    //cout << mail_path << endl;
    //Verifizierung des Pfades
    struct dirent *direntp;
    DIR *dirp;

    if ((dirp = opendir(mail_path)) == NULL) {
        cerr << "Failed to open directory: \"" << path << "\"" << endl;
    }
    else{
        while ((direntp = readdir(dirp)) != NULL) {
            if(strncmp(direntp->d_name, ".", 1) != 0 ){ //Die Verifzierungsdatei ist versteckt und beginnt daher mit einem .
                continue;                               //Alles, das nicht mit einem . beginnt wird übersprungen
            }
            if(strcmp(direntp->d_name, ".maildir") == 0){ //Verzeichnis muss eine versteckte Datei .maildir enthalten, um legitim sein zu können.
                mail_dir_exists = true;
            }
        }
    }
    while ((closedir(dirp) == -1) && (errno == EINTR));

    //delete[] mail_path;
    return mail_dir_exists;
}

bool dir_handler::user_dir_exists(string username){

    string user_path_string = make_absolute_user_path(username);
    const char* user_path = new char[user_path_string.length()+1];
    user_path = user_path_string.c_str();

    bool user_dir_exists = false;

    //cout << mailPath << endl;
    //Verifizierung des Pfades
    DIR *dirp;

    if ((dirp = opendir(user_path)) == NULL) {
        cerr << "Failed to open directory: \"" << user_path << "\"" << endl;
    }
    else{
        user_dir_exists = true;
    }

    while ((closedir(dirp) == -1) && (errno == EINTR));


    //delete[] user_path;
    return user_dir_exists;
}

void dir_handler::make_user_dir(string username){

    string user_path_string = make_absolute_user_path(username);
    const char* user_path = new char[user_path_string.length()+1];
    user_path = user_path_string.c_str();

    if(mkdir(user_path, 0777) != 0){//Unsauber, ungenau. Gefällt mir nicht (777)
        cerr << "error creating user directory" << endl;
    }
    //delete[] user_path;
}
