main.o : src/main.cpp 
		g++ -g src/main.cpp src/Services.h src/page_table.h src/page_table.cpp src/pte.h src/TLBuffer.h src/TLBuffer.cpp -o main.o

.PHONY: main.o clean

clean:
		rm *.o
			rm *.txt

