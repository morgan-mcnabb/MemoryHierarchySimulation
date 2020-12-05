main.o : main.cpp 
		g++ -g main.cpp Services.h page_table.h page_table.cpp pte.h TLBuffer.h TLBuffer.cpp -o main.o

clean:
		rm *.o
			rm *.txt

