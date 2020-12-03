all : main.o
	.PHONY : all

main.o : main.cpp 
		g++ -g main.cpp page_table.cpp pte.cpp -o main.o

clean:
		rm *.o
			rm *.txt

