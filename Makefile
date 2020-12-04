main.o : main.cpp 
		g++ -g main.cpp page_table.h pte.cpp -o main.o

clean:
		rm *.o
			rm *.txt

