CC=g++
CFLAGS=-O2 -g
LDFLAGS=-lX11 -lXext
SOURCES=main.cpp x.cpp options.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=slop
BINDIR="/usr/bin"

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

install: all
	mkdir -p $(DESTDIR)/$(BINDIR)
	cp $(CURDIR)/$(EXECUTABLE) $(DESTDIR)$(BINDIR)
