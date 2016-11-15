CC = g++
DEBUG = -g
STD = -std=c++11
THREADFLAG = -pthread 
CFLAGS = -Wextra -pedantic $(STD) $(DEBUG) $(THREADFLAG) -c
LFLAGS = -Wextra -pedantic $(STD) $(DEBUG) $(THREADFLAG)
OBJ = ThreadPool.o concur01.o
INCLUDE = ThreadPool.hpp

all: TestThreadPool

TestThreadPool: $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) \
TestThreadPool.cpp -lboost_unit_test_framework -o TestThreadPool.o

concur01.o: concur01.cpp concur01.hpp $(INCLUDE)
	$(CC) $(CFLAGS) concur01.cpp -o concur01.o

ThreadPool.o: ThreadPool.cpp $(INCLUDE)
	$(CC) $(CFLAGS) ThreadPool.cpp -o ThreadPool.o

test: justatest.cpp $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) \
        justatest.cpp -o test

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) $< -o $@

test: test

link: ThreadPool.o

clean:
	\rm *.o *~ testSpace/*.a
