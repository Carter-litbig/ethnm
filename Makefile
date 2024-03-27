CC=g++
CFLAGS=-g
TARGET:nmsrv
LIBS=-lpthread
OBJS=client_db.o \
	 		client_service.o \
			connection_manager.o  \
			tcp_server.o \
			network_utils.o \
			tcp_client.o \
			ethnm.o \
			ByteCircularBuffer.o \
			msg_delimiter.o \
			msg_delimiter_fixed.o \
			msg_delimiter_var.o \

nmsrv:nmsrv.o ${OBJS}
	${CC} ${CFLAGS} ${OBJS} nmsrv.o -o nmsrv ${LIBS}
	
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

ethnm.o:ethnm.cpp
	${CC} ${CFLAGS} -c ethnm.cpp -o ethnm.o	

ByteCircularBuffer.o:ByteCircularBuffer.cpp
	${CC} ${CFLAGS} -c ByteCircularBuffer.cpp -o ByteCircularBuffer.o	

msg_delimiter.o:msg_delimiter.cpp
	${CC} ${CFLAGS} -c msg_delimiter.cpp -o msg_delimiter.o	

msg_delimiter_fixed.o:msg_delimiter_fixed.cpp
	${CC} ${CFLAGS} -c msg_delimiter_fixed.cpp -o msg_delimiter_fixed.o		

msg_delimiter_var.o:msg_delimiter_var.cpp
	${CC} ${CFLAGS} -c msg_delimiter_var.cpp -o msg_delimiter_var.o

clean:
	rm -f *.o
	rm -f nmsrv
