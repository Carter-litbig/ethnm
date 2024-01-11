CC=g++
CFLAGS=-g
TARGET:tcpsrv.exe
LIBS=-lpthread
OBJS=client_db.o \
	 		client_service.o \
			connection_manager.o  \
			tcp_server.o \
			network_utils.o

tcpsrv.exe:tcpsrv.o ${OBJS}
	${CC} ${CFLAGS} ${OBJS} tcpsrv.o -o tcpsrv.exe ${LIBS}
	
client_db.o:client_db.cpp
	${CC} ${CFLAGS} -c client_db.cpp -o client_db.o

client_service.o:client_service.cpp
	${CC} ${CFLAGS} -c client_service.cpp -o client_service.o

connection_manager.o:connection_manager.cpp
	${CC} ${CFLAGS} -c connection_manager.cpp -o connection_manager.o

tcp_server.o:tcp_server.cpp
	${CC} ${CFLAGS} -c tcp_server.cpp -o tcp_server.o

network_utils.o:network_utils.cpp
	${CC} ${CFLAGS} -c network_utils.cpp -o network_utils.o

clean:
	rm -f *.o
	rm -f *exe
