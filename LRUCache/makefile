CC = g++
DEBUG = -g
STD = -std=c++11
CFLAGS = -Wextra -pedantic $(STD) $(DEBUG) -c
LFLAGS = -Wextra -pedantic $(STD) $(DEBUG)

LRUCache.o: LRUCache.cpp LRUCache.hpp
	$(CC) $(CFLAGS) LRUCache.cpp -o LRUCache.o

TestLRUCache.o: TestLRUCache.cpp LRUCache.hpp LRUCache.o
	$(CC) $(LFLAGS) LRUCache.o \
TestLRUCache.cpp -lboost_unit_test_framework -o TestLRUCache.o

appLRU: appLRU.cpp LRUCache.hpp LRUCache.o
	$(CC) $(LFLAGS) appLRU.cpp LRUCache.o -o appLRU

test: TestLRUCache.o

clean:
	\rm *.o *~
