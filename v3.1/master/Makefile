CC=g++

SHDIR=../common

#-I/user/include/mysql -L/usr/lib
#$<依赖文件,$@目标文件

CFLAGS= -I $(SHDIR) -I/usr/include/mysql -L/usr/lib -lpthread -lmysqlclient

MASTER=master_epoll

OBJS = master.o $(SHDIR)/common.o

RM = rm -f

all : $(MASTER)

.PHONY: clean

$(MASTER): $(OBJS)
	$(CC)  -o $@ $(OBJS) $(CFLAGS)

$(OBJS): %.o : %.c
	$(CC) -c  $< -o $@ $(CFLAGS)

clean:
	$(RM) $(OBJS) $(MASTER)
