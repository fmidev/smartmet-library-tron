PROG = $(patsubst %.cpp,%,$(wildcard *Test.cpp))

INCS = $(wildcard ../include/*.h)

-include $(HOME)/.smartmet.mk
GCC_DIAG_COLOR ?= always
CXX_STD ?= c++11

# Boost 1.69

ifneq "$(wildcard /usr/include/boost169)" ""
  INCLUDES += -I/usr/include/boost169
  LIBS += -L/usr/lib64/boost169
endif


MAINFLAGS = -std=$(CXX_STD) -Wall -W -Wno-unused-parameter

EXTRAFLAGS = -Werror -pedantic -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Winline \
	-Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-pmf-conversions \
	-Wsign-promo -Wchar-subscripts -Wold-style-cast \
	-Wshadow -Woverloaded-virtual

DIFFICULTFLAGS = -Weffc++ -Wunreachable-code -Wredundant-decls

CC = g++
CFLAGS = -DUNIX -MD -O0 -g $(MAINFLAGS)

INCLUDES += -I../tron -I/usr/include/smartmet
LIBS += -L.. -lsmartmet-tron -lsmartmet-gis -L/usr/geos38/lib64 -lgeos

all: $(PROG)
clean:
	rm -f $(PROG) *.d *~

test: $(PROG)
	@echo Running tests:
	@rm -f *.err
	@for prog in $(PROG); do \
	  ( ./$$prog || touch $$prog.err ) ; \
	done
	@test `find . -name \*.err | wc -l` = "0" || ( echo ; echo "The following tests have errors:" ; \
		for i in *.err ; do echo `basename $$i .err`; done ; rm -f *.err ; false ) 

$(PROG) : % : %.cpp $(INCS)
	$(CC) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)

-include *.d
