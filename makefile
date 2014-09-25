CXX ?= g++
CXXFLAGS ?= -O2 -g
LDFLAGS += -lX11 -lXext -lrt
SOURCES = main.cpp x.cpp options.cpp rectangle.cpp
OBJECTS = $(SOURCES:.cpp=.o)
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

