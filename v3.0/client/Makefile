CC = g++

OJBS = client.o common.o

CLIENT = client_v3.0_debug

RM = rm -f 

.PHONY: clean

$(CLIENT): $(OJBS)
	$(CC) -o $(CLIENT) $(OJBS) -lz -pthread

client.o: client_v3.0.c ../common/common.h
	$(CC) -c client_v3.0.c -o client.o -lz -pthread

common.o: ../common/common.c ../common/common.h
	$(CC) -c ../common/common.c -o common.o -lpthread

clean: 
	$(RM) $(OJBS)
