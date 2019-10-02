SUBNAME = tron
LIB = smartmet-$(SUBNAME)
SPEC = smartmet-library-$(SUBNAME)
INCDIR = smartmet/$(SUBNAME)

# Note: Must not use -Ofast or similar which disable infinity handling

FLAGS = -std=c++11 -fPIC -MD -Wall -W -Wno-unused-parameter -Wno-variadic-macros

FLAGS_RELEASE = -DNDEBUG -O2 -g

FLAGS_DEBUG = -O0 -g \
        -Werror \
        -Wpointer-arith \
        -Wcast-qual \
        -Wcast-align \
        -Wwrite-strings \
        -Wconversion \
        -Wctor-dtor-privacy \
        -Wnon-virtual-dtor \
        -Wno-pmf-conversions \
        -Wsign-promo \
        -Wchar-subscripts \
        -Wredundant-decls \
        -Woverloaded-virtual

CC = g++
ARFLAGS = -r

# Default compiler flags

DEFINES = -DUNIX

CFLAGS_RELEASE = $(DEFINES) $(FLAGS) $(FLAGS_RELEASE)
CFLAGS_DEBUG   = $(DEFINES) $(FLAGS) $(FLAGS_DEBUG)

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
  override CFLAGS += $(CFLAGS_DEBUG)
else
  override CFLAGS += $(CFLAGS_RELEASE)
endif

LDFLAGS = 

INCLUDES = -I$(includedir)

LIBS = -L$(libdir)

# Installation directories

processor := $(shell uname -p)

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

ifeq ($(processor), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

bindir = $(PREFIX)/bin
includedir = $(PREFIX)/include
datadir = $(PREFIX)/share
objdir = obj

# Compilation directories

vpath %.cpp $(SUBNAME)
vpath %.h $(SUBNAME)
vpath %.o obj

# How to install

LIBFILE = lib$(LIB).a

INSTALL_PROG = install -m 775
INSTALL_DATA = install -m 664

# The files to be compiled

SRCS = $(wildcard $(SUBNAME)/*.cpp)
HDRS = $(wildcard $(SUBNAME)/*.h)
OBJS = $(patsubst %.cpp, obj/%.o, $(notdir $(SRCS)))

INCLUDES := -Iinclude $(INCLUDES)

# For make depend:

.PHONY: test rpm

# The rules

all: objdir $(LIBFILE)
debug: objdir $(LIBFILE)
release: objdir $(LIBFILE)

$(LIBFILE): $(OBJS)
	$(AR) $(ARFLAGS) $(LIBFILE) $(OBJS)

clean:
	rm -f $(LIBFILE) $(SUBNAME)/*~
	rm -rf obj
	+cd test && make clean

format:
	clang-format -i -style=file $(SUBNAME)/*.h $(SUBNAME)/*.cpp test/*.cpp

install:
	@mkdir -p $(includedir)/$(INCDIR)
	@list='$(HDRS)'; \
	for hdr in $$list; do \
	  HDR=$$(basename $$hdr); \
	  echo $(INSTALL_DATA) $$hdr $(includedir)/$(INCDIR)/$$HDR; \
	  $(INSTALL_DATA) $$hdr $(includedir)/$(INCDIR)/$$HDR; \
	done
	@mkdir -p $(libdir)
	$(INSTALL_PROG) $(LIBFILE) $(libdir)/$(LIBFILE)

test:
	+cd test && make test

objdir:
	@mkdir -p obj

rpm: clean $(SPEC).spec
	rm -f $(SPEC).tar.gz # Clean a possible leftover from previous attempt
	tar -czvf $(SPEC).tar.gz --transform "s,^,$(SPEC)/," *
	rpmbuild -ta $(SPEC).tar.gz
	rm -f $(SPEC).tar.gz

.SUFFIXES: $(SUFFIXES) .cpp

obj/%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

-include $(wildcard obj/*.d)
