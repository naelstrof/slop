CXX ?= g++
CXXFLAGS ?= -O2 -g
LDFLAGS += -lX11 -lXext -lrt
SOURCES = main.cpp x.cpp cmdline.c rectangle.cpp
OBJECTS = main.o x.o cmdline.o rectangle.o
EXECUTABLE = slop
BINDIR = "/usr/bin"

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJECTS)

dist-clean: clean
	$(RM) $(EXECUTABLE)

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(CURDIR)/$(EXECUTABLE) $(DESTDIR)$(BINDIR)

