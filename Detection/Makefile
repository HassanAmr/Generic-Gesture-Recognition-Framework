CC=g++
IDIR =/usr/local/include/GRT/
LDIR =/usr/lib
LIBS=-lgrt
LFLAGS=-O0 -I$(IDIR) -L$(LDIR)
DEBUG = -g -ggdb
STD=gnu++11

# #put in LIBS later

all: Detection.cpp
	$(CC) $(DEBUG) Detection.cpp $(LFLAGS) $(LIBS) -o detect -std=$(STD)

clean:
	rm detect

	