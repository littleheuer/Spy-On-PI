CC = g++

SHDIR=../common

CFLAGS= -Wall -I $(SHDIR) -lz -lpthread

OJBS = client0316.o $(SHDIR)/common.o

CLIENT = client0331

RM = rm -f 

.PHONY: clean

$(CLIENT): $(OJBS)
	$(CC) -o $@ $< $(CFLAGS)

$(OBJS): %.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean: 
	$(RM) $(OJBS) $(CLIENT)
