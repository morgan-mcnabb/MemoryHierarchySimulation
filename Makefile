project: main.o TLBuffer.o
	g++ -g -Wall main.o TLBuffer.o -o project

main.o: main.cpp
	g++ -c main.cpp

TLBuffer.o: TLBuffer.cpp TLBuffer.h
	g++ -c TLBuffer.cpp

clean:
	rm *.o project
