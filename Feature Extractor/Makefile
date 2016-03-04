CC=g++
IDIR =
LDIR =
LIBS=
LFLAGS=-O0 -I$(IDIR) -L$(LDIR)
DEBUG = -g -ggdb
STD=gnu++11

#-lgrt #put in LIBS later
#/usr/local/include/GRT/
#/usr/lib

all: Feature_Extractor.cpp
	$(CC) $(DEBUG) Feature_Extractor.cpp $(LFLAGS) $(LIBS) -o FeatureExtractor -std=$(STD)

clean:
	rm detect

	