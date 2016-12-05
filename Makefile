MODULE = tron

FLAGS = -std=c++0x -fPIC -MD -Wall -W -Wno-unused-parameter -Wno-variadic-macros

FLAGS_RELEASE = -DNDEBUG -Ofast -g

FLAGS_DEBUG = -O2 -g \
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
includedir = $(PREFIX)/include/smartmet
datadir = $(PREFIX)/share
objdir = obj

# rpm variables

rpmsourcedir=/tmp/$(shell whoami)/rpmbuild

rpmerr = "There's no spec file ($(specfile)). RPM wasn't created. Please make a spec file or copy and rename it into $(specfile)"

rpmversion := $(shell grep "^Version:" $(MODULE).spec  | cut -d\  -f 2 | tr . _)
rpmrelease := $(shell grep "^Release:" $(MODULE).spec  | cut -d\  -f 2 | tr . _)

rpmexcludevcs := $(shell tar --help | grep -m 1 -o -- '--exclude-vcs')

# Compilation directories

vpath %.cpp source
vpath %.h include
vpath %.o obj

# How to install

LIBFILE = libsmartmet_$(MODULE).a

INSTALL_PROG = install -m 775
INSTALL_DATA = install -m 664

# The files to be compiled

SRCS = $(wildcard source/*.cpp)
HDRS = $(wildcard include/*.h)
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
	rm -f $(LIBFILE) include/*~ source/*~
	rm -rf obj

install:
	@mkdir -p $(includedir)/$(MODULE)
	@list='$(HDRS)'; \
	for hdr in $$list; do \
	  HDR=$$(basename $$hdr); \
	  echo $(INSTALL_DATA) $$hdr $(includedir)/$(MODULE)/$$HDR; \
	  $(INSTALL_DATA) $$hdr $(includedir)/$(MODULE)/$$HDR; \
	done
	@mkdir -p $(libdir)
	$(INSTALL_PROG) $(LIBFILE) $(libdir)/$(LIBFILE)

test:
	+cd test && make test

objdir:
	@mkdir -p obj

rpm: clean
	if [ -e $(MODULE).spec ]; \
	then \
	  smartspecupdate $(MODULE).spec ; \
	  mkdir -p $(rpmsourcedir) ; \
	  tar $(rpmexcludevcs) -C ../ -cf $(rpmsourcedir)/libsmartmet-$(MODULE).tar $(MODULE) ; \
	  gzip -f $(rpmsourcedir)/libsmartmet-$(MODULE).tar ; \
	  TAR_OPTIONS=--wildcards rpmbuild -v -ta $(rpmsourcedir)/libsmartmet-$(MODULE).tar.gz ; \
	  rm -f $(rpmsourcedir)/libsmartmet-$(MODULE).tar.gz ; \
	else \
	  echo $(rpmerr); \
	fi;

tag:
	cvs -f tag 'smartmet_$(MODULE)_$(rpmversion)-$(rpmrelease)' .


.SUFFIXES: $(SUFFIXES) .cpp

obj/%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

ifneq ($(wildcard obj/*.d),)
include $(wildcard obj/*.d)
endif
