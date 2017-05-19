SERVERDIR = ../../server
CXXFLAGS = -O2 -MMD -MP -Wall -I$(SERVERDIR) -std=gnu++17

CLEAN = *.d *.o hrac klient.tar.gz

all: hrac klient.tar.gz
naserveri: hrac

.PHONY: klient.tar.gz
klient.tar.gz:
	tar czf $@ $(addprefix --exclude=,$(CLEAN)) *

hrac: main.o server_marshal.o server_common.o
	$(LINK.cpp) $^ $(LDLIBS) -o $@

server_%.o: $(SERVERDIR)/%.cpp
	$(COMPILE.cpp) -o $@ $<

clean:
	-rm -f $(CLEAN)

-include *.d
