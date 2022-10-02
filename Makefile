all: shell379

shell379: main.o command.o process.o
	g++ -o shell379 main.o command.o process.o
main.o: main.cpp
	g++ -c main.cpp -std=c++11
command.o: command.cpp
	g++ -c command.cpp -std=c++11
process.o: process.cpp
	g++ -c process.cpp -std=c++11
clean:
	rm *.o final