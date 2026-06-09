compiler: main.o
	g++ -o compiler main.o

main.o: main.cpp
	g++ -c main.cpp
