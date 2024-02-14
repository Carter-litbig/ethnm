CC=g++
CFLAGS=-g
TARGET:tcpsrv.exe
LIBS=-lpthread
OBJS=client_db.o \
	 		client_service.o \
			connection_manager.o  \
			tcp_server.o \
			network_utils.o \
			tcp_client.o \
			ByteCircularBuffer.o \
			msg_delimiter.o \
			msg_delimiter_fixed.o

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

tcp_client.o:tcp_client.cpp
	${CC} ${CFLAGS} -c tcp_client.cpp -o tcp_client.o

ByteCircularBuffer.o:ByteCircularBuffer.cpp
	${CC} ${CFLAGS} -c ByteCircularBuffer.cpp -o ByteCircularBuffer.o	

msg_delimiter.o:msg_delimiter.cpp
	${CC} ${CFLAGS} -c msg_delimiter.cpp -o msg_delimiter.o	

msg_delimiter_fixed.o:msg_delimiter_fixed.cpp
	${CC} ${CFLAGS} -c msg_delimiter_fixed.cpp -o msg_delimiter_fixed.o		

clean:
	rm -f *.o
	rm -f *exe
