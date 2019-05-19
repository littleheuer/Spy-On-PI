CC=g++

MASTER=master_v4.0

OBJS = master.o common.o

RM = rm -f

.PHONY: clean

$(MASTER): $(OBJS)
	$(CC) -D _DEBUG -o $(MASTER) $(OBJS) -lpthread -I/usr/include/mysql -L/usr/lib -lmysqlclient

common.o: ../common/common.c ../common/common.h
	$(CC) -D _DEBUG -c ../common/common.c -o common.o -lpthread

master.o: master.c master.h ../common/common.h
	$(CC) -D _DEUBG -c master.c -o master.o -lpthread -I/usr/include/mysql -L/usr/lib -lmysqlclient

clean:
	$(RM) $(OBJS) $(MASTER)
