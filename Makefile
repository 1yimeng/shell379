all: final powervirus

final: main.o command.o process.o
	g++ -o final main.o command.o process.o
main.o: main.cpp
	g++ -c main.cpp -std=c++11
command.o: command.cpp
	g++ -c command.cpp -std=c++11
process.o: process.cpp
	g++ -c process.cpp -std=c++11
powervirus: powervirus.cpp
	g++ powervirus.cpp -o powervirus
clean:
	rm *.o final

# test: test.o command.o
# 	g++ -o test test.o command.o
# test.o: test.cpp
# 	g++ -c test.cpp
# command.o: command.cpp
# 	g++ -c command.cpp
# clean:
# 	rm *.o test