LIBDIR = /usr/local/lib

CXXFLAGS = -g -Wall -O0 -pthread
LDFLAGS = `pkg-config --cflags --libs protobuf` -lz -lpthread -Wl,-rpath -Wl,$(LIBDIR)

BINARIES = CServer CClient
TARGETS = $(BINARIES)

all: $(TARGETS)

whole: $(BINARIES)

CServer: people.pb.h people.pb.cc TCPTaskCallback.h TCPTask.h TCPTask.cc TCPServer.h TCPServer.cc main.cc

CClient: people.pb.h people.pb.cc client.cc

$(BINARIES):
	g++ $(CXXFLAGS) $(filter %.cc, $^) -o $@ $(LDFLAGS)

people.pb.h people.pb.cc: people.proto
	protoc --cpp_out . $<

clean:
	rm -f people.pb.*
	rm -f $(BINARIES)
