OBJECT = main.o server.o client.o log.o compiler.o
CXXFLAGS = -std=c++11
all: $(OBJECT)
	g++ -o main $(OBJECT) -pthread
