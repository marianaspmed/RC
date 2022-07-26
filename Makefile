CC   = g++

all:
	$(CC) -o user_commands.o -c user_commands.cpp
	$(CC) -o user user_commands.o user.cpp

run: user
	./user