g++ library.cc -std=c++11 -g -Wall -c -o library.o
g++ write_fixed_len_pages.cpp -std=c++11 -Wall -g -o write_fixed_len_pages library.o
g++ read_fixed_len_page.cpp -std=c++11 -Wall -g -o read_fixed_len_page library.o
g++ experiment_2_3.cpp -std=c++11 -g -Wall -o experiment_2_3 library.o
