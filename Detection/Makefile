CC=g++
IDIR =.
LDIR =/usr/lib
LIBS=
LFLAGS=-O0 -I$(IDIR) -L$(LDIR)
DEBUG = -g -ggdb
STD=gnu++11

#-lgrt #put in LIBS later

all: Detection.cpp
	$(CC) $(DEBUG) Detection.cpp $(LFLAGS) $(LIBS) -o detect -std=$(STD)

clean:
	rm detect

	