CXXFLAGS = -g -MMD -MP -Wall -Wextra -std=gnu++11

all: server

server: main.o update.o mapa.o util.o klient.o proces.o marshal.o common.o
	$(LINK.cpp) $^ $(LDLIBS) -o $@

clean:
	-rm -f *.d *.o server

-include *.d

