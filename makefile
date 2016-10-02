CC = g++
DEBUG = -g
STD = -std=c++11
THREADFLAG = -pthread 
CFLAGS = -Wextra -pedantic $(STD) $(DEBUG) $(THREADFLAG) -c
LFLAGS = -Wextra -pedantic $(STD) $(DEBUG) $(THREADFLAG)
OBJ = ThreadPool.o SharedQueue.o SharedMap.o
INCLUDE = ThreadPool.hpp SharedQueue.hpp SharedMap.hpp

all: TestThreadPool

TestThreadPool: TestThreadPool.cpp $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) \
TestThreadPool.cpp -lboost_unit_test_framework -o TestThreadPool.o

ThreadPool.o: ThreadPool.cpp $(INCLUDE)
	$(CC) $(CFLAGS) ThreadPool.cpp -o ThreadPool.o

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) $< -o $@

test: TestThreadPool

link: ThreadPool.o

clean:
	\rm *.o *~ *.a
