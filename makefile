CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

pa4 : pa4.o Database.o Table.o
	$(CC) $(LFLAGS) -std=c++11 Database.o Table.o pa4.o -o pa4

pa4.o : pa4.cpp
	$(CC) $(CFLAGS) -std=c++11 pa4.cpp

Database.o : Database.h Database.cpp
	$(CC) $(CFLAGS) -std=c++11 Database.cpp

Table.o : Table.h Table.cpp
	$(CC) $(CFLAGS) -std=c++11 Table.cpp
	
clean:
	\rm *.o pa4
