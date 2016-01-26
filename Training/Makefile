CC=g++
IDIR =/usr/local/include/GRT/
LDIR =/usr/lib
LIBS=-lgrt
LFLAGS=-O0 -I$(IDIR) -L$(LDIR)
DEBUG = -g -ggdb
STD=gnu++11

# #put in LIBS later

all: Train.cpp
	$(CC) $(DEBUG) Train.cpp $(LFLAGS) $(LIBS) -o train -std=$(STD)

clean:
	rm train

	