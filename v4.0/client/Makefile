CC = g++

OJBS = client.o common.o

CLIENT = client_v4.0_debug

RM = rm -f 

.PHONY: clean

$(CLIENT): $(OJBS)
	$(CC) -o $(CLIENT) $(OJBS) -lz -pthread

client_v4.0.o: client_v4.0.c ../common/common.h
	$(CC) -c client_v4.0.c -o client_v4.0.o -lz -pthread

common.o: ../common/common.c ../common/common.h
	$(CC) -c ../common/common.c -o common.o -lpthread

clean: 
	$(RM) $(OJBS)
