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

string login_operation::execute(stringstream& message_stream, dir_handler& this_handler){

    string username;
    string password;
    string last1;

    getline(message_stream, username);
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
    getline(message_stream, password);
    getline(message_stream, last1);

    if(last1.compare(".")!=0){
        return "ERR(Protocol was violated)\n";
    }

    /**
        AN DIESER STELLE MUSS DIE LDAP AUTHENTIFIZIERUNG ERFOLGEN
        TEMPORÄR HARDCODEN
    */

    if(password == "null"){
        this_handler.set_username(username);

        return "OK\nWelcome, " + username + "!\n";
    }
    else{
        return "ERR\n";
    }

}


string send_operation::execute(stringstream& message_stream, dir_handler& this_handler){

    if(false == this_handler.user_logged_in()){
        return "ERR\n(No user is logged in)";
    }

    string sender;
    string receiver;
    string subject;
    string line;

    string content;

    string last4;

    //Sender, Receiver and Subject are all delimited by \n
    getline(message_stream, sender);
    /**
        KONTROLLEN FÜR ALLE EINGABEN
    */
    getline(message_stream, receiver);
    getline(message_stream, subject);

    //Der Rest ist Inhalt + \n.\n
    while(!message_stream.eof()){
        getline(message_stream, line);
        content += line;
        content += "\n";
    }

    //cout << content;

    //Die letzten 3 Zeichen herausziehen und auf Korrektheit überprüfen \n.\n
    last4 = content.substr(content.length()-4);

    if(last4.compare("\n.\n\n")!=0){
        return "ERR\n(Protocol was violated)";
    }

    content = content.substr(0,content.rfind("\n.\n"));


    /**
        komtrollen: SIND ZU LÖSCHEN

    cout << "Sender: " << sender << endl;
    cout << "Receiver: " << receiver << endl;
    cout << "Subject: " << subject << endl;
    cout << "Content: " << content << endl;
    */



    if(this_handler.user_dir_exists(receiver)==false){
        cout << "Could not find user directory, therefore creating one" << endl;
        this_handler.make_user_dir(receiver);
    }
    //PSEUDO

    //make mail from strings
    mail new_mail = mail::make_new_mail(sender, receiver, subject, content);
    //save mail to file
    string userpath = this_handler.make_absolute_user_path(receiver);
    bool save_succesful = new_mail.save_to_file(userpath);

    //PASST



    return (save_succesful) ? "OK\n" : "ERR(could not save file)\n";
    /*



    dir_ops send_operator;
    if(send_operator.user_dir_exists("./mails/" + receiver) == false){ //hässlicher Hardcode.
        cout << "Verzeichnis wird angelegt" << endl;
        send_operator.make_user_dir("./mails/" + receiver);
    }
    else
        cout << "Verzeichnis existiert bereits" << endl;*
    if(send_operator.user_dir_exists("./mails/" + receiver) == false){ //hässlicher Hardcode.
        return "ERR\n (Receiver does not exist)\n";
    }


    *
    *
    int mail_id = 1;

    string path = "./mails/";
    path += receiver;

    char* path_chars = new char[path.length()+1];
    strcpy(path_chars, path.c_str());

    //char *mailPath = new char[];
    char *mailPath = canonicalize_file_name(path_chars);

    delete[] path_chars;

    struct dirent *direntp;
    DIR *dirp;

    if ((dirp = opendir(mailPath)) == NULL) {
        cerr << "Failed to open directory: " << path << endl;
    }
    else{
        while ((direntp = readdir(dirp)) != NULL) {

            if (strncmp(direntp->d_name, ".", 1) == 0           //Links überpringen
                || strncmp(direntp->d_name, "..", 2) == 0
                || strncmp(direntp->d_name, "0.", 2) == 0) {    //Dummy Mail überspringen
                continue;
            }
            else{
                string saved_mail_name(direntp->d_name);
                int temp = stoi(saved_mail_name.substr(0, saved_mail_name.find('.')));
                if(temp > mail_id){
                    mail_id = temp;
                }
            }
        }
        while ((closedir(dirp) == -1) && (errno == EINTR));
        free(mailPath);
    }

    *
    *
    string filename = "./mails/";
    filename += receiver;
    filename += "/";
    //filename +=   TODO: DATUM UND UHRZEIT
    //filename
    filename += to_string(++mail_id);
    filename += ". ";
    filename += sender;
    filename += " sub:";
    filename += subject;

    char *mailPath = canonicalize_file_name(filename.c_str());

    filename = mailPath;*
    filename += ".txt";

    cout << "name of file to be created" << filename << endl;

    fstream newmail_file;
    newmail_file.open (filename, ios::out);
    newmail_file << message;
    newmail_file.close();

    //delete[] mailPath;
    return "OK\n";*/
}

string read_operation::execute(stringstream& message_stream, dir_handler& this_handler){

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

    /*string receiver;
    string sender;
    string subject;
    string message;
    string mail_id;
    string last3;

    bool found_mail = false;

    last3 = message_str.substr(message_str.length()-3);

    if(last3.compare("\n.\n")!=0){
        return "ERR\n (Protocol was violated)";
    }

    receiver = message_str.substr(0, message_str.find('\n'));
    message_str = message_str.substr(message_str.find('\n')+1, message_str.length());
    if(receiver.length() > 8){ //wird in späterer Folge mit Regex ersetzt{
        return "ERR\n (Username is longer than 8)\n";
    }

    mail_id = message_str.substr(0, message_str.rfind("\n.\n"));

    cout << "username: " << receiver << " and mail_id: " << mail_id << endl;

    *

    *

    string path = "./mails/";
    path += receiver;

    char* path_chars = new char[path.length()+1];
    strcpy(path_chars, path.c_str());

    //char *mailPath = new char[];
    char *mailPath = canonicalize_file_name(path_chars);

    delete[] path_chars;

    struct dirent *direntp;
    DIR *dirp;

    if(mailPath==NULL){
        return "ERR\n (User does not exist)";
    }

    if ((dirp = opendir(mailPath)) == NULL) {
        return "ERR\n (Internal server error)";
    }
    else{
        while ((direntp = readdir(dirp)) != NULL) {

            if (strncmp(direntp->d_name, ".", 1) == 0           //Links überpringen
                || strncmp(direntp->d_name, "..", 2) == 0
                || strncmp(direntp->d_name, "0.", 2) == 0) {    //Dummy Mail überspringen
                continue;
            }
            else{
                string mailname(direntp->d_name);   //Sender und Betreff aus Dateinamen
                string file_name(mailPath);         //Rest aus dem Inhalt
                    file_name += "/";
                    file_name += mailname;
                int compare_id_len = message_str.find(".")-1;
                string compare_id = mailname.substr(0, compare_id_len);
                if(mail_id.compare(compare_id) == 0){
                    found_mail = true;
                    *
                        Sammeln von allem was ich brauche:
                        Sender
                        Betreff

                        Inhalt
                    *
                    mailname = message_str.substr(mailname.find(" ", 0)+1);
                    int sender_len = mailname.find(' ');
                    sender = mailname.substr(0, sender_len);
                    mailname = message_str.substr(mailname.find(" ", 0)+1);
                    //string delimiter = "sub:";
                    int subject_len = mailname.length()-8; //-4 wegen sub: und -4 wegen .txt
                    subject = mailname.substr(4, subject_len);

                    cout << "Sender of mail: " << sender << " with Subject of mail: " << subject << endl;

                    ifstream readmail;
                    string message_line;


                    readmail.open(file_name, ios :: in);

                    if(readmail.is_open() != true)
                    {
                        readmail.close();
                        return "ERR\n (Internal server error)";
                    }
                    else{
                        while(!readmail.eof()){
                            getline(readmail, message_line);
                            message += message_line;
                            message += "\n";
                        }
                        readmail.close();
                        //message = mail_stream.str();
                    }

                }

            }
        }
        while ((closedir(dirp) == -1) && (errno == EINTR));
        free(mailPath);
    }

    string response = "FROM: " + sender + "\n"+ "TO: " + receiver + "\n"+ "SUBJECT: " + subject + "\n"+ message;

    if(found_mail){
        return "OK\n" + response;
    }
    else{
        return "ERR\n (Specified mail does not exist)";
    }*/
}

string list_operation::execute(stringstream& message_stream, dir_handler& this_handler){

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

    /*string username;
    string last3;
    stringstream mail_stream;

    last3 = message_str.substr(message_str.length()-3);

    if(last3.compare("\n.\n")!=0){
        return "ERR\n (Protocol was violated)";
    }

    username = message_str.substr(0, message_str.find('\n'));
    message_str = message_str.substr(message_str.find('\n')+1, message_str.length());
    if(username.length() > 8){ //wird in späterer Folge mit Regex ersetzt{
        return "ERR\n (Username is longer than 8)\n";
    }

    int mail_count = 0;

    string path = "./mails/";
    path += username;

    char* path_chars = new char[path.length()+1];
    strcpy(path_chars, path.c_str());

    //char *mailPath = new char[];
    char *mailPath = canonicalize_file_name(path_chars);

    delete[] path_chars;

    struct dirent *direntp;
    DIR *dirp;

    if(mailPath==NULL){
        return "ERR\n (User does not exist)";
    }

    if ((dirp = opendir(mailPath)) == NULL) {
        return "ERR\n (Internal server error)";
    }
    else{
        while ((direntp = readdir(dirp)) != NULL) {

            if (strncmp(direntp->d_name, ".", 1) == 0           //Links überpringen
                || strncmp(direntp->d_name, "..", 2) == 0
                || strncmp(direntp->d_name, "0.", 2) == 0) {    //Dummy Mail überspringen
                continue;
            }
            else{
                ++mail_count;
                string mailname(direntp->d_name);
                string delimiter = "sub:";
                string subject;
                string mail_id;
                mail_id = mailname.substr(0, mailname.find(" "));
                int subject_len = mailname.find(".txt")-(mailname.find(delimiter)+delimiter.length());
                subject = mailname.substr(mailname.find(delimiter)+delimiter.length(), subject_len); //-4 wegen .txt, delimiter.length() wegen "sub:"
                cout << "SUBJECT OF HANDLED MAIL: " << subject << endl;
                mail_stream << mail_id << subject << "\n";

            }
        }
        while ((closedir(dirp) == -1) && (errno == EINTR));
        free(mailPath);
    }

    *
        Anmerkung:
        Die retournierte Nachricht ist noch unsortiert. Sprich die folgende Ausgabe ist möglich:

        1. Betreff
        4. Betreff
        2. Betreff
        3. Betreff

        Verschoben auf später weil nicht falsch, sondern nur unschön.
    *

    return "\n" + to_string(mail_count) + "\n" + mail_stream.str();*/
}

string delete_operation::execute(stringstream& message_stream, dir_handler& this_handler){

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


    bool delete_succesful;
    for(const mail& mail_obj: this_handler.get_mail_list()){
        if(mail_obj.get_mail_id() == stoi(mail_id)){
            string userpath = this_handler.make_absolute_user_path(receiver);
            delete_succesful = mail_obj.delete_file(userpath);
            if(delete_succesful){
                this_handler.create_mail_list(receiver);
            }
        }
    }

    return (delete_succesful) ? "OK\n" : "ERR\n";
    /*bool delete_okay = false;


    string receiver;
    string sender;
    string mail_id;
    string last3;

    last3 = message_str.substr(message_str.length()-3);

    if(last3.compare("\n.\n")!=0){
        return "ERR\n (Protocol was violated)";
    }

    receiver = message_str.substr(0, message_str.find('\n'));
    message_str = message_str.substr(message_str.find('\n')+1, message_str.length());
    if(receiver.length() > 8){ //wird in späterer Folge mit Regex ersetzt{
        return "ERR\n (Username is longer than 8)\n";
    }

    mail_id = message_str.substr(0, message_str.rfind("\n.\n"));

    cout << "username: " << receiver << " and mail_id: " << mail_id << endl;


    string path = "./mails/";
    path += receiver;

    char* path_chars = new char[path.length()+1];
    strcpy(path_chars, path.c_str());

    //char *mailPath = new char[];
    char *mailPath = canonicalize_file_name(path_chars);

    delete[] path_chars;

    struct dirent *direntp;
    DIR *dirp;

    if(mailPath==NULL){
        return "ERR\n (User does not exist)";
    }

    if ((dirp = opendir(mailPath)) == NULL) {
        return "ERR\n (Internal server error)";
    }
    else{
        while ((direntp = readdir(dirp)) != NULL) {

            if (strncmp(direntp->d_name, ".", 1) == 0           //Links überpringen
                || strncmp(direntp->d_name, "..", 2) == 0
                || strncmp(direntp->d_name, "0.", 2) == 0) {    //Dummy Mail überspringen
                continue;
            }
            else{
                string mailname(direntp->d_name);   //Sender und Betreff aus Dateinamen
                string file_name(mailPath);         //Rest aus dem Inhalt
                    file_name += "/";
                    file_name += mailname;
                int compare_id_len = message_str.find(".")-1;
                string compare_id = mailname.substr(0, compare_id_len);
                if(mail_id.compare(compare_id) == 0){
                    if(remove(file_name.c_str())==0){
                        delete_okay = true;
                    }
                }

            }
        }
        while ((closedir(dirp) == -1) && (errno == EINTR));
        free(mailPath);
    }



    if(delete_okay){
        return "OK\n";
    }
    else{
        return "ERR\n";
    }*/

}

string quit_operation::execute(stringstream& message_stream, dir_handler& this_handler){

    if(false == this_handler.user_logged_in()){
        return "ERR\n(No user is logged in)";
    }

    return "GOOD BYE!\n";
}
