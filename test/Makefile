PROG = $(patsubst %.cpp,%,$(wildcard *Test.cpp))

REQUIRES = geos

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

FLAGS = -std=$(CXX_STD) -Wall -W -Wno-unused-parameter

ifeq ($(TSAN), yes)
  FLAGS += -fsanitize=thread
endif
ifeq ($(ASAN), yes)
  FLAGS += -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fsanitize-address-use-after-scope
endif

CFLAGS = -DUNIX -DUSE_UNSTABLE_GEOS_CPP_API -O0 -g $(FLAGS) -Wno-write-strings

INCLUDES += -I../tron \

LIBS += ../libsmartmet-tron.a -lboost_filesystem -lboost_chrono $(REQUIRED_LIBS)

all: $(PROG)
clean:
	rm -f $(PROG) *~

test: $(PROG)
	@echo Running tests:
	@rm -f *.err
	@for prog in $(PROG); do \
	  ( ./$$prog || touch $$prog.err ) ; \
	done
	@test `find . -name \*.err | wc -l` = "0" || ( echo ; echo "The following tests have errors:" ; \
		for i in *.err ; do echo `basename $$i .err`; done ; rm -f *.err ; false )

$(PROG) : % : %.cpp ../libsmartmet-tron.a Makefile
	$(CXX) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)
