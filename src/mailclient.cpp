#include "../headers/mailclient.h"

void print_usage(){
    std::cerr << "Usage is: ./mailserver -i(ip adress) -p(port) [-h]" << std::endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    /*signal(SIGINT, shutdown);
    signal(SIGHUP, shutdown);
    signal(SIGQUIT, shutdown);*/



    //Startparameters
    int option_char=-1;
    int port=-1;
    std::string ip_string;
    bool help_needed = false;

    while((option_char = getopt(argc, argv, "i:p:h")) != EOF ){
        switch(option_char)
        {
            case 'p':{port = atoi(optarg);
                      if(port<=1024){
                        std::cerr << "Port must not be a well kown port(1024 or smaller)." << std::endl;
                        exit(1);
                      }
                      break;}
            case 'i':{ip_string = optarg;                                              //HIER -> escapter '\'
                      if(ip_string == "localhost"){
                            ip_string = "127.0.0.1";
                            break;
                      }
                      std::regex ip_regex("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
                      if(!std::regex_match(ip_string, ip_regex)){
                        std::cerr << "Entered IP adress cannot be valid." << std::endl;
                        exit(1);
                      }
                      break;}
            case 'h':{help_needed = true;
                      break;}
            case '?':{help_needed = true;
                      break;}
            default:{assert(0);}

        }
    }
    if( help_needed ||
        optind > argc+1 ||
        argc==1 ||
        port <0 ||
        ip_string.empty()){
            print_usage();
        }

    mailclient client(port, ip_string);
    client.run();

    return 0;
}
