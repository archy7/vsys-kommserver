#include "../headers/UI.h"
#include "../headers/mailclient.h"


#include <termios.h> //asterisks for password
#include <unistd.h>
#include <regex>

using namespace std;

UI::UI(){

}

UI::~UI(){

}

string UI::prompt_menu_option(list<client_operation*>& server_op_list){

	cout << "Choose what you wish to do" << endl;
	for( auto ptr : server_op_list){
		//cout << ptr->get_name() << endl;
		cout << ptr->name << endl;
	}
	cout << ">>" << endl;


	string op_wish;
	getline(cin, op_wish);
	transform(op_wish.begin(), op_wish.end(), op_wish.begin(), ::toupper);

	return op_wish;
}

int UI::getch(){

    int ch;
    struct termios t_default, t_hidden;

    tcgetattr(STDIN_FILENO, &t_default);
    t_hidden = t_default;
    t_hidden.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_hidden);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_default);

    return ch;
}

string UI::prompt_username(){

	string input;
	std::regex user_regex("^([i][f][1][4][b][0])([0][1-9]|[1-7][0-9])$");

	cout << "Enter your username." << endl;

	while(1){
		getline(cin, input);
		if(!regex_match(input, user_regex)){
			cout << "The entered user names has an invalid format." << endl
			<< "Please re-enter." << endl;
			continue;
		}
		else{
			break;
		}
	}

	return input;
}

string UI::prompt_password(bool asterisks){

    const char BACKSPACE = 127;
    const char RETURN = 10;

    string password;
    unsigned char ch=0;

    while(RETURN!=(ch=UI::getch())){
        if(BACKSPACE == ch){
            if(0!=password.length()){
                if(asterisks){
                    cout <<"\b \b";
                }
                password.resize(password.length()-1);
            }
        }
        else{
            password += ch;
            if(asterisks){
                cout << '*';
            }
        }
    }

    cout << endl;

    return password;
}

string UI::prompt_receiver_list(){

	string input;
	string receivers;
	std::regex user_regex("^([i][f][1][4][b][0])([0][1-9]|[1-7][0-9])$");

	cout << "Enter the receivers. (Enter a . to stop):" << endl;

	while(1){
		getline(cin, input);
		if(input == "."){
			break;
		}
		if(!regex_match(input, user_regex)){
			cout << "At least one of the entered user names has an invalid format." << endl
			<< "Please re-enter." << endl;
			continue;
		}
		receivers += input;
		receivers += ";";
	}

	return input;
}

string UI::prompt_subject(){

	const int max_len = 80;
	string input = "";

	cout << "Enter the subject:" << endl;

	while(input.empty() || input.length() > max_len){
		if(input.size() > max_len)
			cout << "The entered subject is too long (maximum of " << max_len << " characters)" << endl
			<< "Please re-enter:" << endl;
		else
			cerr << "Something went horribly wrong in UI::prompt_subject. LIKE REALLY WRONG" << endl;

		getline(cin, input);
	}

	return input;
}

string UI::prompt_content(){

    string input = "";
    string content;

    cout << "Enter the content or your message:" << endl;

    while(input != "."){
        getline(cin, input);
        if(input == ".")
            break;

        content += input + "\n";
        input.clear();
    }

    return content;
}

list<string> UI::prompt_attachments(){

	string input = "";
	list<string> filepaths;
	bool attaching = true;

	cout << "Do you want to attach files to your mail? [y/n]" << endl;

	do{
		if(!input.empty()){
			cout << "There is only yes [y] or NONE [n]." << endl;
		}
		getline(cin, input);
	}while("y" != input && "n" != input);

	if(input == "y"){
		cout << "Specify the path to the file that you want to attach:" << endl;
		//<< "Enter . to stoqp adding files(currently only one allowed and possible)" << endl;
		while(attaching){
			getline(cin, input);
			if("." == input){
				break;
			}
			string filepath = dir_handler::make_absolute_path(input);
			if(filepath.empty()){
				cout << "could not find the specified file" << endl;
			}
			else{
				filepaths.push_back(input);//(filepath);
				attaching = false; //Das hier sorgt dafür, dass nur eine Datei angehängt werden kann
			}
		}

	}

	return filepaths;
}

unsigned int UI::prompt_mail_id(){

	//TODO: Error handling. what if "abc" is entered

	int mail_id= 0;
	cout << "Enter the mailnumber: " << endl;

	while(mail_id<1){
		cin >> mail_id;
		if(mail_id < 1){
			cout << "Invalid mailnumber. Mailnumbers must be integers bigger than 0." << endl;
		}
	}

	//TODO: C++ cast
	return (unsigned int) mail_id;

}

int UI::prompt_download_attachment(){

	//TODO: Error handling. Only all/ALL or NONE/NONE or a positive integer are allowed

	string input;
	int i_input= 0;

	cout << "Do you wish do download attachments? [all/attachment number/none]" << endl;

	while(input.empty()){
		getline(cin, input);
		transform(input.begin(), input.end(), input.begin(), ::toupper);

		if(input == "ALL "){
			i_input = -1;
			break;
		}
		else if(input == "NONE"){
			i_input = 0;
			break;
		}
		i_input = stoi(input); //unsicher
		if(i_input >0){
			break;
		}
		input.clear();
	}

	return i_input;
}

void UI::print_mail(const mail& this_mail){

    cout   	<< "Sender: " << this_mail.sender << "\n"
			<< "Receiver: " << this_mail.receiver << "\n"
			<< "Subject: " << this_mail.subject << "\n"
			<< "Number of attachments: " << this_mail.attachment_count << "\n";

    int i = 1;
    for( string attachment_name : this_mail.get_attachment_names()){
        cout << "Attachment #" << i << ": " << attachment_name << "\n";
    }

    cout   << "Content: " << "\n" << this_mail.content;

}
