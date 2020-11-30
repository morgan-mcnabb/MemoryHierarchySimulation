OBJS = main.o
SOURCE = main.cpp
#HEADER = 
OUT = project
CC = g++
FLAGS = -g -c -Wall
#LFLAGS = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) 

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp

clean:
	rm -f $(OBJS) $(OUT)
