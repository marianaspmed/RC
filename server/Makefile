CC   = g++

all:
	$(CC) -o server_commands.o -c server_commands.cpp
	$(CC) -o DS server_commands.o server.cpp

run: DS
	./DS