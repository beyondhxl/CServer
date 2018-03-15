CC = g++

SRC = ../src

CSERVER_INCLUDE = /root/beyond/CServer

CFLAGS = -g -O0 -Wall -Wextra -Werror \
		   -Wconversion -Wno-unused-parameter \
		   -Wold-style-cast -Woverloaded-virtual \
		   -Wpointer-arith -Wshadow -Wwrite-strings \
		   -march=native -rdynamic \
		   -I$(CSERVER_INCLUDE)
		   -std=c++11

LDFLAGS = -lprotobuf

all: CServer CClient clean

CServer: CServer.o CClient.o
	$(CC) -o $@ $^ $(LDFLAGS)

CServer.o: $(SRC)/server.cpp
	$(CC) -c $(CFLAGS) $<

CClient.o: $(SRC)/client.cpp
	$(CC) -c $(CFLAGS) $<

.PHONY: clean

clean:
	rm *.o
