CC=g++
CFLAGS=-O2 -g
LDFLAGS=-lX11 -lXext
SOURCES=main.cpp x.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=slrn

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
