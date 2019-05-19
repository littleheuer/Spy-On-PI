CC = g++

SHDIR=../common

CFLAGS= -Wall -I $(SHDIR) -lz -lpthread

OJBS = client.o $(SHDIR)/common.o

CLIENT = client_v3.1

RM = rm -f 

.PHONY: clean

$(CLIENT): $(OJBS)
	$(CC) -o $@ $< $(CFLAGS)

$(OBJS): %.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean: 
	$(RM) $(OJBS) $(CLIENT)
