CC = g++
DEBUG = -g
STD = -std=c++11
CFLAGS = -Wextra -pedantic -O0 $(STD) $(DEBUG) -c
LFLAGS = -Wextra -pedantic -O0 $(STD) $(DEBUG)
CFLAGS_COV = $(CFLAGS) --coverage
LFLAGS_COV = $(LFLAGS) --coverage

OUTPUT_FILE = LRUCache_cov.info

LRUCache.o: LRUCache.cpp LRUCache.hpp
	$(CC) $(CFLAGS) LRUCache.cpp -o LRUCache.o

TestLRUCache.o: TestLRUCache.cpp LRUCache.hpp LRUCache.o
	$(CC) $(LFLAGS) LRUCache.o \
TestLRUCache.cpp -lboost_unit_test_framework -o TestLRUCache.o

test: TestLRUCache.o

all: test

check: TestLRUCache.o
	./TestLRUCache.o

coverage:
	# Generate appLRU test coverage html report
	# cleaning...
	- rm *.o *~ *.gcno *.gcda
	# building LRUCache_cov.o...
	$(CC) $(CFLAGS_COV) LRUCache.cpp -o LRUCache_cov.o
	# building TestLRUCache_cov.o...
	$(CC) $(LFLAGS_COV) LRUCache_cov.o \
TestLRUCache.cpp -lboost_unit_test_framework -o TestLRUCache_cov.o
	# zerocounter...
	lcov -b . -d . -z --q
	# testing...
	./TestLRUCache_cov.o
	# capture...
	lcov -b . -d . -c -o $(OUTPUT_FILE)
	# remove library include from the report
	lcov --remove $(OUTPUT_FILE) "/usr*" -o $(OUTPUT_FILE)
	# cleaning...
	-rm -rf ../../test_cov
	# generating html ../../test_cov report...
	genhtml -o ../../test_cov -f -t "LRU Cache test coverage" --num-spaces 4 $(OUTPUT_FILE)

clean:
	\rm *.o *~ *.gcno *.gcda
