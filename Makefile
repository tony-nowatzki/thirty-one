all:
	/s/gcc-4.8.2/bin/g++ -g -ggdb -gdwarf-3 -lboost_program_options -O3 -std=c++11 main.cpp -o thirty-one
debug:
	/s/gcc-4.8.2/bin/g++ -g -ggdb -gdwarf-3 -lboost_program_options -O0 -std=c++11 main.cpp -o thirty-one

clean:
	rm -f thirty-one
