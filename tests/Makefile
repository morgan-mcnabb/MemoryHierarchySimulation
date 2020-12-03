all: test_main.o
	.PHONY: all

test_main.o: test_main.cpp page_table_tests.cpp tlb_tests.cpp data_cache_tests.cpp
	g++ -g -Wall test_main.cpp page_table_tests.cpp tlb_tests.cpp data_cache_tests.cpp -o test_main.o



