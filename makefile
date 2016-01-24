CC = g++
CFLAGS = -Wall -pedantic -g -std=c++11
EXECS = mailserver mailclient
LDAPFLAGS = -lldap -DLDAP_DEPRECATED -llber
#######

all: ${EXECS}
mailserver: mailserver.o mailserver_lib.o mails.o server_operation.o dir_handler.o server_assets.o communication.o 
	${CC} ${CFLAGS} -o mailserver mailserver.o mailserver_lib.o mails.o server_operation.o dir_handler.o server_assets.o communication.o
mailclient: mailclient.o mailclient_lib.o client_operation.o mails.o dir_handler.o UI.o communication.o client_assets.o
	${CC} ${CFLAGS} -o mailclient mailclient.o mailclient_lib.o mails.o client_operation.o dir_handler.o UI.o communication.o client_assets.o

mailserver.o: src/mailserver.cpp headers/mailserver.h 
	${CC} ${CFLAGS} -o mailserver.o -c src/mailserver.cpp
mailserver_lib.o: src/mailserver_lib.cpp headers/mailserver.h
	${CC} ${CFLAGS} -o mailserver_lib.o -c src/mailserver_lib.cpp
server_assets.o: src/server_assets.cpp headers/server_assets.h
	${CC} ${CFLAGS} -o server_assets.o -c src/server_assets.cpp ${LDAPFLAGS}
mails.o: src/mails.cpp headers/mails.h
	${CC} ${CFLAGS} -o mails.o -c src/mails.cpp
server_operation.o: src/server_operation.cpp headers/server_operation.h
	${CC} ${CFLAGS} -o server_operation.o -c src/server_operation.cpp 
dir_handler.o: src/dir_handler.cpp headers/dir_handler.h
	${CC} ${CFLAGS} -o dir_handler.o -c src/dir_handler.cpp
communication.o: src/communication.cpp headers/communication.h
	${CC} ${CFLAGS} -o communication.o -c src/communication.cpp
##
mailclient.o: src/mailclient.cpp headers/mailclient.h
	${CC} ${CFLAGS} -o mailclient.o -c src/mailclient.cpp
mailclient_lib.o: src/mailclient_lib.cpp headers/mailclient.h
	${CC} ${CFLAGS} -o mailclient_lib.o -c src/mailclient_lib.cpp
client_operation.o: src/client_operation.cpp headers/client_operation.h
	${CC} ${CFLAGS} -o client_operation.o -c src/client_operation.cpp
UI.o: 		src/UI.cpp headers/UI.h
	${CC} ${CFLAGS} -o UI.o -c src/UI.cpp
client_assets.o: src/client_assets.cpp headers/client_assets.h
	${CC} ${CFLAGS} -o client_assets.o -c src/client_assets.cpp
########
clean:
	rm -f *.o
clean-all:
	rm -f *.o mailserver mailclient
