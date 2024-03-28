CC=g++
CFLAGS=-g
<<<<<<< HEAD
<<<<<<< HEAD
TARGET:tcpsrv.exe
=======
TARGET:nmsrv
>>>>>>> 7aae719 (add deleted file)
=======
TARGET:nmsrv
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9
LIBS=-lpthread
OBJS=client_db.o \
	 		client_service.o \
			connection_manager.o  \
			tcp_server.o \
			network_utils.o \
			tcp_client.o \
<<<<<<< HEAD
<<<<<<< HEAD
			ByteCircularBuffer.o \
			msg_handler.o

tcpsrv.exe:tcpsrv.o ${OBJS}
	${CC} ${CFLAGS} ${OBJS} tcpsrv.o -o tcpsrv.exe ${LIBS}
=======
			ethnm.o \
			ByteCircularBuffer.o \
			msg_delimiter.o \
			msg_delimiter_fixed.o \
			msg_delimiter_var.o \

nmsrv:nmsrv.o ${OBJS}
	${CC} ${CFLAGS} ${OBJS} nmsrv.o -o nmsrv ${LIBS}
>>>>>>> 7aae719 (add deleted file)
=======
			ethnm.o \
			ByteCircularBuffer.o \
			msg_delimiter.o \
			msg_delimiter_fixed.o \
			msg_delimiter_var.o \

nmsrv:nmsrv.o ${OBJS}
	${CC} ${CFLAGS} ${OBJS} nmsrv.o -o nmsrv ${LIBS}
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9
	
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

<<<<<<< HEAD
<<<<<<< HEAD
ByteCircularBuffer.o:ByteCircularBuffer.cpp
	${CC} ${CFLAGS} -c ByteCircularBuffer.cpp -o ByteCircularBuffer.o	

msg_handler.o:msg_handler.cpp
	${CC} ${CFLAGS} -c msg_handler.cpp -o msg_handler.o	
=======
=======
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9
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
<<<<<<< HEAD
>>>>>>> 7aae719 (add deleted file)
=======
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9

clean:
	rm -f *.o
	rm -f nmsrv
