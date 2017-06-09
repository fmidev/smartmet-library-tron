PROG = $(patsubst %.cpp,%,$(wildcard *Test.cpp))

INCS = $(wildcard ../include/*.h)

MAINFLAGS = -std=c++0x -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
	-Wshadow -Woverloaded-virtual

DIFFICULTFLAGS = -Weffc++ -Wunreachable-code -Wredundant-decls

CC = g++
CFLAGS = -DUNIX -MD -O0 -g $(MAINFLAGS)
INCLUDES = -I ../tron -I /usr/include/smartmet
LIBS = -L .. -lsmartmet-tron -lsmartmet-gis -lgeos

all: $(PROG)
clean:
	rm -f $(PROG) *.d *~

test: $(PROG)
	@echo Running tests:
	@for prog in $(PROG); do \
	./$$prog; \
	done

$(PROG) : % : %.cpp ../libsmartmet-tron.a $(INCS)
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)

-include *.d
