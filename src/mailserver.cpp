#include "../headers/mailserver.h"
#include "../headers/dir_handler.h"

#include <assert.h>

void print_usage(){
    std::cerr << "Usage is: ./mailserver -p[value] -d[value] [-h]" << std::endl;
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
    //bool create_dir = false;
    std::string directory;
    bool help_needed = false;

    //dir_handler directory_operator("bla");

    while((option_char = getopt(argc, argv, "p:d:h")) != EOF ){ //"cp:d:h"
        switch(option_char)
        {
            case 'p':{port = atoi(optarg);
                      if(port<=1024){
                        std::cerr << "Port must not be a well kown port(1024 or smaller)." << std::endl;
                        exit(1);
                      }
                      break;}
            /*case 'c':{if(directory_operator.dir_exists(directory) ==  false){
                        directory_operator.make_dir(directory);
                        //TODO: FUNKTION IS NOT NICHT IMPLEMENTIERT
                        std::cout << "C set" << std::endl;
                      }
                      break;}*/
            /*case 'd':{directory = optarg;
                      if(directory_operator.dir_exists(directory) ==  false){
                        std::cerr << "Mail directory does not exist. Exiting." << std::endl;
                        exit(1);
                      }
                      break;}*/
            case 'd':{directory = optarg;
                      if(dir_handler::dir_exists(directory) ==  false){
                        std::cerr << "Mail directory does not exist. Exiting." << std::endl;
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
        directory.empty()){
            print_usage();
        }

    /**
        Server startet. Architektur:
        Es wird ein Server instanziert. Der Server listened auf einem Socket, für jeden Socket wird ein Datensocket geöffnet.
        Der Listen-Socket ist (wird) iterativ, der Datensocket parallel.
    */

    mailserver server = mailserver::make_mailserver(port, directory);
    server.run();

    return 0;
}
