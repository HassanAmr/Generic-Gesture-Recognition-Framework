CC=g++
IDIR =.
LDIR =/usr/lib
LIBS=-lmraa
DEBUG = -g -ggdb
CFLAGS=-I$(IDIR) -L$(LDIR) -O0 -c -Wall
LFLAGS=-I$(IDIR) -L$(LDIR) -O0 -Wall
STD=gnu++11

#-lgrt #put in LIBS later
#-Wall

all: sensors

sensors:	Sensors.o BNO055.o
	$(CC) $(DEBUG) Sensors.o BNO055.o $(LFLAGS) $(LIBS) -o sensors -std=$(STD)

Sensors.o: Sensors.cpp
	$(CC) $(DEBUG) Sensors.cpp $(CFLAGS) $(LIBS) -std=$(STD)

BNO055.o: BNO055.cpp
	$(CC) $(DEBUG) BNO055.cpp $(CFLAGS) $(LIBS) -std=$(STD)

clean:
	rm -rf *o detect	