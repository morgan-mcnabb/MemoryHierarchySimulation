test_main.o: test_main.cpp page_table_tests.cpp tlb_tests.cpp data_cache_tests.cpp ../page_table.h ../page_table.cpp ../TLBuffer.cpp ../Services.h
	g++ -g -Wall test_main.cpp page_table_tests.cpp tlb_tests.cpp data_cache_tests.cpp services_tests.cpp ../page_table.h ../page_table.cpp ../TLBuffer.h ../TLBuffer.cpp ../Services.h -o test_main.o

clean:
	rm *.o

