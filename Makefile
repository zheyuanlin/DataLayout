all: library.o write_fixed_len_pages read_fixed_len_page experiment_2_3

library.o: library.cc
	g++ library.cc -std=c++11 -g -Wall -c -o library.o

write_fixed_len_pages: write_fixed_len_pages.cpp library.o
	g++ write_fixed_len_pages.cpp -std=c++11 -Wall -g -o write_fixed_len_pages library.o

read_fixed_len_page: read_fixed_len_page.cpp library.o
	g++ read_fixed_len_page.cpp -std=c++11 -Wall -g -o read_fixed_len_page library.o
	
experiment_2_3: experiment_2_3.cpp library.o
	g++ experiment_2_3.cpp -std=c++11 -g -Wall -o experiment_2_3 library.o
