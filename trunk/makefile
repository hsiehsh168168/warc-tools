# -*- makefile -*-

# -------------------------------------------------------------------  #
# Copyright (c) 2007-2008 Hanzo Archives Limited.                      #
#                                                                      #
# Licensed under the Apache License, Version 2.0 (the "License");      #
# you may not use this file except in compliance with the License.     #
# You may obtain a copy of the License at                              #
#                                                                      #
#     http://www.apache.org/licenses/LICENSE-2.0                       #
#                                                                      #
# Unless required by applicable law or agreed to in writing, software  #
# distributed under the License is distributed on an "AS IS" BASIS,    #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      #
# implied.                                                             #
# See the License for the specific language governing permissions and  #
# limitations under the License.                                       #
#                                                                      #
# You may find more information about Hanzo Archives at                #
#                                                                      #
#     http://www.hanzoarchives.com/                                    #
#                                                                      #
# You may find more information about the WARC Tools project at        #
#                                                                      #
#     http://code.google.com/p/warc-tools/                             #
# -------------------------------------------------------------------  #

UNAME_S  := $(shell uname -s 2>/dev/null || echo unknown)
UNAME_O  := $(shell uname -o 2>/dev/null || echo unknown)

VERSION  := $(shell cat version)


APP      = app
TST      = utest
DOC      = doc
LIB      = lib
MISC     = misc
PUBLIC   = $(LIB)/public
PRIVATE  = $(LIB)/private
PLUGIN   = $(PRIVATE)/plugin
CUNIT    = $(PLUGIN)/cunit
GZIP     = $(PLUGIN)/gzip
TIGER    = $(PLUGIN)/tiger
EVENT    = $(PLUGIN)/event
APPYTHON = $(APP)/python
EVENT_COMPACT = $(EVENT)/compat
REGEX    = $(PLUGIN)/regex
OSDEP    = $(PRIVATE)/os
MINGW_DEP= $(OSDEP)/mingw
OUT      = $(TST)/outputs
PYTHON   = $(PLUGIN)/python
HEADERS  = -I. -I$(PRIVATE) -I$(PUBLIC) -I$(GZIP) \
		   -I$(CUNIT)       -I$(TIGER)  -I$(EVENT) \
		   -I$(EVENT_COMPACT) -I$(REGEX) -I$(PYTHON)

MAJOR     = 0
MINOR     = 17
RELEASE   = 96

LIBSUFFIX = so
LIBNAME   = libwarc
SONAME	  =,$(LIBNAME).$(LIBSUFFIX).$(MAJOR)
SHAREDNAME=$(LIBNAME).$(LIBSUFFIX).$(MAJOR).$(MINOR).$(RELEASE)

NAME     = warc-tools
PROJECT  = $(NAME)-$(MAJOR).$(MINOR)
PREFIX	 = /usr/local
DESTDIR	 = $(PROJECT)$(PREFIX)/$(NAME)


####################
# Lighttpd mod_warc
####################

# Follow the instructions for how to install "mod_cgi" and "mod_fcgi" 
# on the "doc/install" document. Lighttpd needs to be installed before.

LIGHTY_DESTDIR = /var/www
LIGHTTPD       = $(PLUGIN)/lighttpd
LIGHTY_SRC     = $(LIGHTTPD)/modcgi.c $(LIGHTTPD)/modfcgi.c
INC_LIGHTY     = -I/usr/local/include
LIB_FCGI       = -L/usr/local/lib -lfcgi



####################
# Apache mod_warc
####################

# Follow the instructions for how to install "mod_warc" on the "doc/install"
# document. Apache needs to be installed before trying to install "mod_warc".

APACHE_PREFIX =$(PREFIX)
APACHE_DIR    =$(APACHE_PREFIX)/apache2
APXS          =$(APACHE_DIR)/bin/apxs
APACHECTL     =$(APACHE_DIR)/apachectl
APACHE        =$(PLUGIN)/apache
mod_apache    =$(APACHE)/mod_warc

###################
# python wrapper
###################

SWIG            = swig
#INC_PYTHON      = -I$(shell echo /usr/include/python*.*)
INC_PYTHON      = -I$(shell python -c 'import distutils.sysconfig; print distutils.sysconfig.get_python_inc();')

WPYNAME   	= _warc
WPYSONAME	=,$(WPYNAME).$(LIBSUFFIX).$(MAJOR)
WPYSHAREDNAME   = $(WPYNAME).$(LIBSUFFIX)
APYNAME   	= _arc
APYSONAME	=,$(APYNAME).$(LIBSUFFIX).$(MAJOR)
APYSHAREDNAME   = $(APYNAME).$(LIBSUFFIX)


###############
# flags
###############

MAKE	  = make

AR		  = ar
RANLIB	  = ranlib

GCC       = gcc
CC	  	  = $(GCC) $(HEADERS)

DFLAG     = -g

CFLAGS    = -Wall -W -Wunused -ansi -pedantic -Werror -Wno-long-long \
			-Wunused-function -std=gnu89 $(OPT)

DEFS      =  -DSTDC_HEADERS=1 -DHAVE_STRING_H=1 -DHAVE_ALLOCA_H=1 -DEBUG=1



#CFLAGS  += -Wunreachable-code 
#CFLAGS  += -Wconversion -Wtraditional -Wmissing-prototypes -Wshadow

# comment the line below to compile without large file support (i.e. 64 bits offsets)
CFLAGS  += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES

# compile in release mode (no debug info will be included and assertions are disabled). Use with care.
ifeq ($(W_RELEASE),on)
	DFLAG   =
	CFLAGS += -NDEBUG
endif
CFLAGS += $(DFLAG)

# uncomment the line below to compile with optimization enabled
#CFLAGS += -O3 -march=i686 -pipe

# uncomment the line below to disable all assertions and traces
#CFLAGS += -DNDEBUG

# uncomment the line below to disable all printing via "w_fprintf"
#CFLAGS += -Dw_fprintf=""

# uncomment the lines below to allocate bigger (or lesser) buffer for Gzip compression (or uncompression)
#CFLAGS += -DWARC_GZIP_IN_BUFFER_SIZE=32768
#CFLAGS += -DWARC_GZIP_OUT_BUFFER_SIZE=65536

# GCC extra flags
GCC_EXTRA = -Wextra

# OS dependant functions (for portability issue)
MKTEMP = $(OSDEP)/wmktmp
CSAFE  = $(OSDEP)/wcsafe

# LIBEVENT DEPS
EV_UTEST_BIN = $(TST)/server  	    $(TST)/client
EV_UTEST_SRC = $(TST)/server.c      $(TST)/client.c
EV_APP_BIN   = $(APP)/warcserver  	$(APP)/warcclient
EV_APP_SRC   = $(APP)/warcserver.c  $(APP)/warcclient.c
EV_SRC       = $(EVENT)/event.c		$(EVENT)/buffer.c    $(EVENT)/log.c \
		       $(EVENT)/evutil.c    $(EVENT)/http.c	     $(EVENT)/signal.c \
			   $(EVENT)/select.c    $(EVENT)/min_heap.c
##$(EVENT)/evbuffer.c $(EVENT)/event_tagging.c $(EVENT)/evrpc.c


SHARED_OS    = shared_unix
PYSHARED_OS	 = pyshared_unix

# compile WARC as a shared library
ifeq ($(W_SHARED),on)
	S_CFLAGS = -fPIC -DPIC
endif

# compile the Python wrapper as a shared library
ifeq ($(W_PYSHARED),on)
	S_CFLAGS = -fPIC -DPIC
endif

# adapt makefile to OS
ifeq ($(UNAME_S),Linux)
	CFLAGS      += -pedantic-errors
	EV_OS   	 = $(EVENT)/os/linux
	HEADERS     += -I$(OSDEP) -I$(EV_OS) -I$(CUNIT)/os/linux
	EV_SRC		+= $(EVENT)/epoll.c $(EVENT)/poll.c $(EVENT)/strlcpy.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = -lrt 
#-lnsl -lresolv
endif
ifeq ($(UNAME_S),FreeBSD)
	MAKE         = gmake
	EV_OS   	 = $(EVENT)/os/freebsd
	HEADERS     += -I$(OSDEP) -I$(EV_OS) -I$(CUNIT)/os/freebsd
	EV_SRC		+= $(EVENT)/kqueue.c $(EVENT)/poll.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = 
	INC_PYTHON     = -I$(shell echo /usr/local/include/python*.*)
endif
ifeq ($(UNAME_S),OpenBSD)
	MAKE         = gmake
	CFLAGS      += -DWINLINE=""
	EV_OS        = $(EVENT)/os/openbsd
	HEADERS     += -I$(OSDEP) -I$(EV_OS) -I$(CUNIT)/os/openbsd
	EV_SRC		+= $(EVENT)/kqueue.c $(EVENT)/poll.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = 
	GCC_EXTRA    = 
	INC_PYTHON     = -I$(shell echo /usr/local/include/python*.*)
endif
ifeq ($(UNAME_S),NetBSD)
	MAKE         = gmake
	EV_OS        = $(EVENT)/os/netbsd
	HEADERS     += -I$(OSDEP) -I$(EV_OS) -I$(CUNIT)/os/netbsd
	EV_SRC		+= $(EVENT)/kqueue.c $(EVENT)/poll.c
	EVENT_CONFIG = $(EVENT)/config.h $(EVENT)/event-config.h
	EV_LIB		 = 
	INC_PYTHON     = -I$(shell echo /usr/local/include/python*.*)
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS 		+= -pedantic
	LDFLAGS		 = -Wl,-flat_namespace
	EV_OS        = $(EVENT)/os/osx
	HEADERS     += -I$(OSDEP) -I$(EV_OS) -I$(CUNIT)/os/osx
	EV_SRC		+= $(EVENT)/kqueue.c $(EVENT)/poll.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = 
#-lresolv
	SHARED_OS	 = shared_osx
	PYSHARED_OS	 = pyshared_osx
	LIBSUFFIX    = dylib
	INSTALLNAME	 = $(LIBNAME).$(MAJOR).$(LIBSUFFIX)
	SHAREDNAME	 = $(LIBNAME).$(MAJOR).$(MINOR).$(RELEASE).$(LIBSUFFIX)
	S_CFLAGS     =
#PYINSTALLNAME= $(PYNAME).$(MAJOR).$(LIBSUFFIX)
	PYLIBSUFFIX	 = so
	WPYSHAREDNAME = $(WPYNAME).$(PYLIBSUFFIX)
	APYSHAREDNAME = $(APYNAME).$(PYLIBSUFFIX)
	SWIG_CFLAGS  = -bundle -undefined suppress -flat_namespace
endif
ifeq ($(UNAME_S),SunOS)
	CC	    += -R/usr/local/lib
	EV_OS        = $(EVENT)/os/solaris
	HEADERS     += -I$(OSDEP) -I$(EV_OS) -I$(CUNIT)/os/solaris
	EV_SRC	    += $(EVENT)/devpoll.c $(EVENT)/poll.c $(EVENT)/evport.c $(EVENT)/strlcpy.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB	     = -lrt -lsocket
	PYSHARED_OS  = pyshared_solaris
	SWIG_LDFLAGS = -G
#-lnsl -lresolv -lsocket
	SONAME	     = 
endif
ifneq ($(findstring MINGW,$(UNAME_S)),)
	MKTEMP       = $(MINGW_DEP)/wmktmp
	CSAFE	     = $(MINGW_DEP)/wcsafe
	EV_OS        = $(EVENT)/os/mingw
	WIN32CODE    = $(EVENT)/WIN32-Code
	HEADERS     := $(HEADERS) -I$(MINGW_DEP) -I$(EV_OS) -I$(WIN32CODE) -I$(CUNIT)/os/mingw
	EV_SRC	     = $(WIN32CODE)/misc.c $(WIN32CODE)/win32.c 
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = -lws2_32
	SHARED_OS    = shared_mingw
	PYSHARED_OS  = pyshared_mingw
	LIBSUFFIX    = dll
	SHAREDNAME   = $(LIBNAME).$(LIBSUFFIX)
	WPYSHAREDNAME = $(WPYNAME).$(LIBSUFFIX)
	APYSHAREDNAME = $(APYNAME).$(LIBSUFFIX)
	S_CFLAGS     = -DBUILD_DLL
	BASH	     = sh
	EV_SRC       =
	EV_UTEST_BIN = 
	EV_UTEST_SRC = 
	EV_APP_BIN   = 
	EV_APP_SRC   = 
	LIGHTY_SRC   =
endif
ifneq ($(findstring CYGWIN,$(UNAME_S)),)
	EV_OS        = $(EVENT)/os/cygwin -I$(CUNIT)/os/cygwin
	HEADERS     := $(HEADERS) -I$(OSDEP) -I$(EV_OS)
	EV_SRC		:= $(EV_SRC) $(EVENT)/poll.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = -lresolv
#-lnsl -lrt
	LIBSUFFIX    = dll
	SHARED_OS    = shared_cygwin
	LIB_PYTHON   = -L$(shell echo /usr/lib/python*.*)
	PY_VERSION   = $(shell echo /usr/lib/python*.* | sed -e "s|.*\(.\..\)|\1|")
	PYSHARED_OS  = pyshared_cygwin
	WPYSONAME	=,$(WPYNAME).$(LIBSUFFIX).$(MAJOR)
	WPYSHAREDNAME   = $(WPYNAME).$(LIBSUFFIX)
	APYNAME   	= _arc
	APYSONAME	=,$(APYNAME).$(LIBSUFFIX).$(MAJOR)
	APYSHAREDNAME   = $(APYNAME).$(LIBSUFFIX)
	S_CFLAGS     = 
endif


# post OS flags processing

CFLAGS += $(GCC_EXTRA)
CFLAGS += $(S_CFLAGS)

RFLAGS = $(CFLAGS)
CFLAGS += $(DEFS)
###############
# sources
###############

b	= $(PRIVATE)/wstring.c   $(PRIVATE)/wclass.c  $(PRIVATE)/wlist.c \
	  $(PRIVATE)/wfile.c     $(PRIVATE)/wrecord.c $(PRIVATE)/wanvl.c \
      $(PRIVATE)/afsmhdl.c   $(PRIVATE)/wfsmanvl.c $(PRIVATE)/wheader.c \
      ${CSAFE}.c             $(PRIVATE)/wgetopt.c $(PRIVATE)/arecord.c \
      $(PRIVATE)/afile.c     ${MKTEMP}.c          $(PRIVATE)/wendian.c \
      $(PRIVATE)/wuuid.c     $(PRIVATE)/wserver.c $(PRIVATE)/whash.c \
      $(PRIVATE)/wkv.c	     $(PRIVATE)/wgzip.c   $(PRIVATE)/wclient.c \
      $(PRIVATE)/wregexp.c   

b	+= $(GZIP)/adler32.c     $(GZIP)/crc32.c      $(GZIP)/deflate.c \
	  $(GZIP)/infback.c      $(GZIP)/inffast.c    $(GZIP)/inflate.c \
	  $(GZIP)/inftrees.c     $(GZIP)/uncompr.c    $(GZIP)/wgzipbit.c \
	  $(GZIP)/zutil.c        $(GZIP)/compress.c   $(GZIP)/trees.c

b	+= $(CUNIT)/Automated.c  $(CUNIT)/Basic.c     $(CUNIT)/CUError.c \
	   $(CUNIT)/Console.c    $(CUNIT)/MyMem.c	  $(CUNIT)/TestDB.c \
	   $(CUNIT)/TestRun.c	 $(CUNIT)/Util.c      $(CUNIT)/menu.c

b	+= $(TIGER)/tiger.c

b   += $(REGEX)/regex.c

b	+=$(EV_SRC)

c	= $(b) \
	  $(TST)/string.c        $(TST)/list.c        $(TST)/anvl.c \
      $(TST)/record.c        $(TST)/file.c  	  $(TST)/arcrecord.c \
      $(TST)/warcgzip.c      $(TST)/warcgunzip.c  $(TST)/arcfile.c \
      $(TST)/a2w.c           $(TST)/uuid.c        $(TST)/getopt.c \
      $(TST)/object.c        $(TST)/hash.c 	      $(EV_UTEST_SRC) \
      $(TST)/header.c        $(TST)/regexp.c


c	+= $(APP)/arc2warc.c     $(APP)/warcdump.c    $(APP)/warcfilter.c \
	  $(APP)/warcvalidator.c $(APP)/warcserver.c  $(APP)/warcclient.c

h	= $(PUBLIC)/wclass.h     $(PUBLIC)/warc.h      $(PRIVATE)/wstring.h \
	  $(PUBLIC)/wrtype.h     $(PUBLIC)/wfile.h     $(PRIVATE)/wlist.h \
	  $(PRIVATE)/wuuid.h     $(PRIVATE)/wgetopt.h  $(PUBLIC)/wrecord.h \
      $(PRIVATE)/wanvl.h     $(PRIVATE)/wmem.h 	   $(PRIVATE)/fsm.h \
      $(PRIVATE)/fsmanvl.h   ${CSAFE}.h            $(PRIVATE)/wkv.h \
      $(PRIVATE)/afsmhdl.h   $(PRIVATE)/arecord.h  $(PRIVATE)/afile.h \
      ${MKTEMP}.h            $(PRIVATE)/arecord.h  $(PRIVATE)/wendian.h \
      $(PRIVATE)/whash.h     $(PRIVATE)/wserver.h  $(PRIVATE)/wclient.h \
	  $(PUBLIC)/wgzip.h $(PUBLIC)/wheader.h $(PRIVATE)/wregexp.h

h   += $(GZIP)/crc32.h       $(GZIP)/deflate.h    $(GZIP)/inffast.h \
	  $(GZIP)/inffixed.h     $(GZIP)/inflate.h    $(GZIP)/inftrees.h \
	  $(GZIP)/wgzipbit.h     $(GZIP)/wos.h        $(GZIP)/zconf.h \
	  $(GZIP)/zlib.h         $(GZIP)/zutil.h      $(GZIP)/trees.h

h	+= $(CUNIT)/Automated.h	 $(CUNIT)/Basic.h	  $(CUNIT)/CUCurses.h \
	   $(CUNIT)/CUError.h	 $(CUNIT)/Console.h   $(CUNIT)/MyMem.h \
	   $(CUNIT)/TestDB.h     $(CUNIT)/TestRun.h   $(CUNIT)/Util.h

h	+= $(TIGER)/tiger.h

h   += $(MENU)/menu.h

h   += $(REGEX)/regex.h

h   += $(EVENT)/log.h	  $(EVENT)/event-internal.h	  $(EVENT)/evdns.h \
	  $(EVENT)/event.h	  $(EVENT)/event-config.h	  $(EVENT)/evrpc.h \
	  $(EVENT)/min_heap.h $(EVENT)/strlcpy-internal.h $(EVENT_CONFIG) \
	  $(EVENT)/evutil.h	  $(EVENT)/evsignal.h        $(EVENT)/http-internal.h \
	  $(EVENT)/evhttp.h	  $(EVENT)/evrpc-internal.h \
	  $(EVENT)/compat/sys/queue.h $(EVENT)/compat/sys/_time.h

u	= $(TST)/string       $(TST)/list            $(TST)/anvl \
	  $(TST)/record       $(TST)/uuid            $(TST)/warcgzip \
      $(TST)/warcgunzip   $(TST)/file    	     $(TST)/arcrecord \
      $(TST)/arcfile      $(TST)/a2w 	         $(TST)/getopt \
      $(TST)/hash         $(TST)/object          $(TST)/header \
      $(TST)/regexp

u  += $(EV_UTEST_BIN)

t  += $(u)

a  = $(APP)/arc2warc         $(APP)/warcdump      $(APP)/warcfilter \
	 $(APP)/warcvalidator	 

a  += $(EV_APP_BIN)


t  += $(a)





###############
# library
###############

libwarc = $(b:.c=.o)

obj     = $(libwarc) $(c:.c=.o)

gzlib   = $(GZIP)/adler32.o  $(GZIP)/crc32.o      $(GZIP)/deflate.o \
		  $(GZIP)/infback.o  $(GZIP)/inffast.o    $(GZIP)/inflate.o \
	  	  $(GZIP)/inftrees.o $(GZIP)/uncompr.o    $(GZIP)/wgzipbit.o \
	  	  $(GZIP)/zutil.o    $(GZIP)/compress.o   $(GZIP)/trees.o \
		  $(PRIVATE)/wgzip.o $(PRIVATE)/wendian.o

cunit	= $(CUNIT)/Automated.o  $(CUNIT)/Basic.o     $(CUNIT)/CUError.o \
	   	  $(CUNIT)/Console.o    $(CUNIT)/MyMem.o	 $(CUNIT)/TestDB.o \
	   	  $(CUNIT)/TestRun.o	$(CUNIT)/Util.o		 $(CUNIT)/menu.o

evlib   = $(EV_SRC:.c=.o)

regex   = $(REGEX)/regex.o


all:  	$t

static:	$(libwarc)	; $(AR) cvr $(LIBNAME).a $(libwarc); $(RANLIB) $(LIBNAME).a

# http://rute.2038bug.com/node26.html.gz (Unix, Unix-like)
shared_unix: clean $(libwarc)
		$(CC) -shared -Wl,-soname$(SONAME) -o $(SHAREDNAME) $(libwarc)
		ln -sf $(SHAREDNAME)  $(LIBNAME).$(LIBSUFFIX).$(MAJOR) && \
		ln -sf $(SHAREDNAME)  $(LIBNAME).$(LIBSUFFIX)


# http://www.finkproject.org/doc/porting/shared.php (MacOS X)
shared_osx: clean $(libwarc)
		$(CC) -dynamiclib -install_name $(INSTALLNAME) \
		-compatibility_version $(MAJOR).$(MINOR) \
		-current_version $(MAJOR).$(MINOR).$(RELEASE) \
		-o $(SHAREDNAME) $(libwarc)
		ln -sf $(SHAREDNAME)  $(LIBNAME).$(MAJOR).$(LIBSUFFIX) && \
		ln -sf $(SHAREDNAME)  $(LIBNAME).$(LIBSUFFIX)

#http://www.emmestech.com/software/cygwin/pexports-0.43/moron2.html
# http://mingw.org/docs.shtml
shared_mingw: clean $(libwarc)
		$(CC) -shared -o $(SHAREDNAME) $(libwarc) \
		-Wl,--out-implib,$(LIBNAME)dll.a

# http://www.cygwin.com/cygwin-ug-net/dll.html
shared_cygwin: clean $(libwarc)
		$(CC) -shared -o $(SHAREDNAME) $(libwarc) \
		-Wl,--out-implib,$(LIBNAME)dll.a \
		-Wl,--export-all-symbols \
		-Wl,--enable-auto-import \
		-Wl,--no-whole-archive

shared:
		@$(MAKE) W_SHARED=on $(SHARED_OS)

release:
		@$(MAKE) W_RELEASE=on

source:	shared static python $(a)
		rm -rf $(PROJECT)
		mkdir -p $(DESTDIR)/bin
		mkdir -p $(DESTDIR)/include/compat/sys
		mkdir -p $(DESTDIR)/lib
		cp -f readme $(DESTDIR)
		cp -f license/apache2-license $(DESTDIR)
		cp -f $(a) $(DESTDIR)/bin
		cp -f $(APP)/*.sh $(DESTDIR)/bin
		cp -f $(APPYTHON)/* $(DESTDIR)/bin
		find $(LIB) -name "*.h" -type "f" -exec cp -f '{}' $(DESTDIR)/include \;
		find $(LIB) -name "*.x" -type "f" -exec cp -f '{}' $(DESTDIR)/include \;
		cp -f $(EVENT_CONFIG) $(DESTDIR)/include
		mv $(DESTDIR)/include/_time.h $(DESTDIR)/include/compat/sys
		mv $(DESTDIR)/include/queue.h $(DESTDIR)/include/compat/sys
		cp -rf $(APACHE) $(DESTDIR)
		cp -rf $(LIGHTTPD) $(DESTDIR)
		cp -rf $(PYTHON) $(DESTDIR) && rm -f $(DESTDIR)/python/*.o
		mv $(LIBNAME).a $(DESTDIR)/lib
		mv *$(LIBNAME)*$(LIBSUFFIX)* $(DESTDIR)/lib


tgz:	source
		rm -f $(PROJECT).tar.gz
		cd $(PROJECT) && tar cvf ../$(PROJECT).tar *
		gzip -9 $(PROJECT).tar
		rm -rf $(PROJECT)

install:tgz
		@echo "-----------------------------------------------------------"
		@echo "Install \"$(NAME)\" library under \"$(PREFIX)\" directory. "
		@echo "YOU MUST BE \"root\"."
		@echo "-----------------------------------------------------------"
		tar xzvf $(PROJECT).tar.gz -C /

deinstall:tgz
		@echo
		@echo "------------------------------------------------------------"
		@echo "De-install \"$(NAME)\" from \"$(PREFIX)\" directory."
		@echo "YOU MUST BE \"root\"."
		@echo "------------------------------------------------------------"
		for i in $(shell tar tzf $(PROJECT).tar.gz); do \
			j="/$$i"; \
			if [ -f "$$j" ]; then \
				rm -f $$j; \
			fi; \
		done


deb:	source
		rm -f $(PROJECT).deb
		cp -rf $(MISC)/DEBIAN $(PROJECT)/
		dpkg-deb --build  $(PROJECT)
		rm -rf $(PROJECT)

rpm:	deb
		rm -f $(PROJECT).rpm
		alien -r $(PROJECT).deb
		rm -f $(PROJECT).deb


mod_apache_install: install mod_apache
		@echo
		@echo "------------------------------------------------------------"
		@echo "Install \"mod_warc\" for Apache2 web server."
		@echo "YOU MUST BE \"root\"."
		@echo "------------------------------------------------------------"
		$(APXS) -i -a $(mod_apache).la
mod_apache:mod_apache_clean
		$(APXS) -I$(PREFIX)/$(NAME)/include -L$(PREFIX)/$(NAME)/lib \
		-c $(mod_apache).c 


mod_lighttpd_install: install mod_lighttpd
		@echo
		@echo "------------------------------------------------------------"
		@echo "Install \"mod_warc\" for Lighttpd web server."
		@echo "YOU MUST BE \"root\"."
		@echo "------------------------------------------------------------"
		cp -f $(LIGHTTPD)/warc.cgi $(LIGHTY_DESTDIR)
		cp -f $(LIGHTTPD)/warc.fcgi $(LIGHTY_DESTDIR)
mod_lighttpd:mod_lighty_clean $(LIGHTTPD)/warc.cgi $(LIGHTTPD)/warc.fcgi



test_all:	;	@$(BASH) $(TST)/test.sh $(u)
test:   $(u) test_all tclean

doc:        ;   doxygen ./doc/warcdoc

.c.o:
	  	${CC} ${CFLAGS} -o $@ -c $<



##################
# unit tests deps
##################

string	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(TST)/string.o \
		  ${CSAFE}.o	         $(cunit)

list	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  ${CSAFE}.o             $(TST)/list.o 			$(cunit)

anvl	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wanvl.o \
		  $(TST)/anvl.o          $(PRIVATE)/wfsmanvl.o  ${CSAFE}.o\
		  $(cunit)

record	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  $(PRIVATE)/wanvl.o     $(PRIVATE)/wrecord.o   $(PRIVATE)/wuuid.o \
          $(PRIVATE)/wfsmanvl.o  ${CSAFE}.o    $(PRIVATE)/wheader.o \
          $(TIGER)/tiger.o       ${MKTEMP}.o            $(TST)/record.o \
	      $(cunit)

header  = $(PRIVATE)/wheader.o   $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o \
          ${CSAFE}.o             $(PRIVATE)/wfsmanvl.o  $(PRIVATE)/wlist.o  \
          $(PRIVATE)/wanvl.o     $(TST)/header.o        $(cunit)

warcgzip  = $(PRIVATE)/wclass.o  $(gzlib)               $(TST)/warcgzip.o \
		  ${CSAFE}.o             $(cunit)

warcgunzip= $(PRIVATE)/wclass.o  $(gzlib)               $(TST)/warcgunzip.o \
		  ${CSAFE}.o             $(cunit)

object	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  $(PRIVATE)/wanvl.o     $(TST)/object.o        ${CSAFE}.o 

file    = $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
	  	  $(PRIVATE)/wanvl.o     $(PRIVATE)/wrecord.o   $(PRIVATE)/wfile.o \
          $(PRIVATE)/wuuid.o     $(PRIVATE)/wheader.o   $(PRIVATE)/wfsmanvl.o \
          ${CSAFE}.o             $(gzlib)               ${MKTEMP}.o \
	      $(TST)/file.o          $(TIGER)/tiger.o       $(cunit)

arcrecord = $(PRIVATE)/arecord.o $(PRIVATE)/wstring.o   $(PRIVATE)/wclass.o \
          ${CSAFE}.o             $(PRIVATE)/wrecord.o   $(PRIVATE)/afile.o \
          $(PRIVATE)/wanvl.o     $(PRIVATE)/wlist.o     $(PRIVATE)/wfsmanvl.o  \
          $(PRIVATE)/wheader.o   $(PRIVATE)/afsmhdl.o   ${MKTEMP}.o \
          $(TST)/arcrecord.o	 ${gzlib}               $(cunit)

a2w    = $(PRIVATE)/wclass.o     $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
	 	 $(PRIVATE)/wanvl.o      $(PRIVATE)/wrecord.o   $(PRIVATE)/wfsmanvl.o  \
         $(PRIVATE)/wheader.o    $(PRIVATE)/wfile.o     $(PRIVATE)/afsmhdl.o \
         ${CSAFE}.o              $(PRIVATE)/arecord.o   $(PRIVATE)/afile.o \
	 	 $(gzlib)                ${MKTEMP}.o \
	 	 $(TST)/a2w.o            $(cunit)

arcfile = $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
	  	 ${CSAFE}.o              $(PRIVATE)/wanvl.o     $(PRIVATE)/afile.o \
         $(PRIVATE)/wfsmanvl.o   $(PRIVATE)/wheader.o   $(PRIVATE)/wrecord.o \
         $(PRIVATE)/arecord.o    $(PRIVATE)/afsmhdl.o   ${MKTEMP}.o \
		 $(TST)/arcfile.o        $(gzlib) 	            $(cunit)

uuid	= $(PRIVATE)/wclass.o    $(PRIVATE)/wuuid.o     ${CSAFE}.o \
		 $(PRIVATE)/wstring.o    $(TIGER)/tiger.o       $(TST)/uuid.o\
         $(cunit)

getopt	= $(PRIVATE)/wclass.o    $(PRIVATE)/wgetopt.o   ${CSAFE}.o \
		  $(PRIVATE)/wstring.o   $(TST)/getopt.o        $(cunit)

hash	= $(PRIVATE)/wclass.o    $(PRIVATE)/whash.o   	${CSAFE}.o \
		  $(PRIVATE)/wlist.o	 $(PRIVATE)/wstring.o   $(PRIVATE)/wkv.o \
		  $(TST)/hash.o          $(cunit)

server =  $(PRIVATE)/wclass.o	 $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  $(PRIVATE)/wanvl.o 	 $(PRIVATE)/wrecord.o   $(PRIVATE)/wheader.o \
	      $(PRIVATE)/wfile.o	 $(PRIVATE)/wuuid.o     $(PRIVATE)/wfsmanvl.o \
		  $(PRIVATE)/wserver.o	 ${CSAFE}.o             $(gzlib) \
		  ${MKTEMP}.o 			 $(TIGER)/tiger.o       $(evlib) \
		  $(TST)/server.o        $(PRIVATE)/wregexp.o   $(regex)

client =  $(PRIVATE)/wclass.o	 $(PRIVATE)/wstring.o  $(PRIVATE)/wlist.o \
		  $(PRIVATE)/wanvl.o     $(PRIVATE)/wrecord.o  $(PRIVATE)/wheader.o \
	      $(PRIVATE)/wfile.o	 $(PRIVATE)/wuuid.o    $(PRIVATE)/wfsmanvl.o \
		  $(PRIVATE)/wclient.o 	 ${CSAFE}.o            $(gzlib) \
		  ${MKTEMP}.o 			 $(TIGER)/tiger.o      $(evlib) \
		  $(TST)/client.o		 $(cunit)

regexp =  $(PRIVATE)/wclass.o    $(PRIVATE)/wregexp.o   $(TST)/regexp.o \
		  ${CSAFE}.o	         $(cunit)               $(regex)



##################
# unit tests deps
##################

$(TST)/string:	  $(string);     $(CC) $(CFLAGS)   -o $@ $(string)
$(TST)/list:	  $(list);       $(CC) $(CFLAGS)   -o $@ $(list)
$(TST)/record:	  $(record);     $(CC) $(CFLAGS)   -o $@ $(record)
$(TST)/object:	  $(object);     $(CC) $(CFLAGS)   -o $@ $(object)
$(TST)/warcgzip:  $(warcgzip);   $(CC) $(CFLAGS)   -o $@ $(warcgzip)
$(TST)/warcgunzip:$(warcgunzip); $(CC) $(CFLAGS)   -o $@ $(warcgunzip)
$(TST)/anvl:	  $(anvl);       $(CC) $(CFLAGS)   -o $@ $(anvl)
$(TST)/file:      $(file);       $(CC) $(CFLAGS)   -o $@ $(file)
$(TST)/arcrecord: $(arcrecord);  $(CC) $(CFLAGS)   -o $@ $(arcrecord)
$(TST)/a2w:       $(a2w);        $(CC) $(CFLAGS)   -o $@ $(a2w)
$(TST)/arcfile:   $(arcfile);    $(CC) $(CFLAGS)   -o $@ $(arcfile)
$(TST)/uuid:	  $(uuid);       $(CC) $(CFLAGS)   -o $@ $(uuid)
$(TST)/getopt:	  $(getopt);     $(CC) $(CFLAGS)   -o $@ $(getopt)
$(TST)/hash:	  $(hash);       $(CC) $(CFLAGS)   -o $@ $(hash)
$(TST)/server:	  $(server);	 $(CC) ${RFLAGS}   -o $@ $(server) $(EV_LIB)
$(TST)/client:	  $(client);	 $(CC) ${CFLAGS}   -o $@ $(client) $(EV_LIB)
$(TST)/header:	  $(header);     $(CC) $(CFLAGS)   -o $@ $(header)
$(TST)/regexp:	  $(regexp);     $(CC) $(RFLAGS)   -o $@ $(regexp) 


######################
# lighttpd cgi module 
######################

modcgi   = $(PRIVATE)/wclass.o	  $(PRIVATE)/wstring.o  $(PRIVATE)/wlist.o \
		   $(PRIVATE)/wanvl.o 	  $(PRIVATE)/wrecord.o  $(PRIVATE)/wheader.o \
	       $(PRIVATE)/wfile.o	  $(PRIVATE)/wuuid.o    ${MKTEMP}.o \
		   $(PRIVATE)/wfsmanvl.o  ${CSAFE}.o	        $(gzlib) \
		   $(PRIVATE)/wgetopt.o   $(TIGER)/tiger.o	    $(LIGHTTPD)/modcgi.o \
           $(PRIVATE)/wregexp.o


$(LIGHTTPD)/warc.cgi:  $(modcgi)
			$(CC) $(RFLAGS) $(INC_LIGHTY) -o $@ $(modcgi)


######################
# lighttpd fcgi module 
######################

modfcgi   = $(PRIVATE)/wclass.o	  $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/wanvl.o 	  $(PRIVATE)/wrecord.o $(PRIVATE)/wheader.o \
	       $(PRIVATE)/wfile.o	  $(PRIVATE)/wuuid.o   $(PRIVATE)/wfsmanvl.o \
		   ${CSAFE}.o	          $(gzlib)             ${MKTEMP}.o \
		   $(PRIVATE)/wgetopt.o   $(TIGER)/tiger.o     $(LIGHTTPD)/modfcgi.o \
           $(PRIVATE)/wregexp.o

$(LIGHTTPD)/warc.fcgi:  $(modfcgi)
			$(CC) $(RFLAGS) $(INC_LIGHTY) -o $@ $(modfcgi) $(LIB_FCGI)



######################
# python SWIG wrapper
######################

pylib   =  $(GZIP)/adler32.o     $(GZIP)/compress.o      $(GZIP)/crc32.o  \
		   $(GZIP)/deflate.o     $(GZIP)/infback.o       $(GZIP)/inffast.o \
		   $(GZIP)/inflate.o     $(GZIP)/inftrees.o      $(GZIP)/trees.o \
		   $(GZIP)/uncompr.o     $(PRIVATE)/wendian.o    $(GZIP)/wgzipbit.o \
	   	   $(GZIP)/zutil.o		 $(PRIVATE)/wgzip.o      $(TIGER)/tiger.o \
		   $(PRIVATE)/wstring.o  $(PRIVATE)/wlist.o		 $(PRIVATE)/wclass.o \
		   $(OSDEP)/wmktmp.o     $(PRIVATE)/whash.o      $(PRIVATE)/wkv.o \
		   $(OSDEP)/wcsafe.o	 $(PRIVATE)/wuuid.o		 $(PRIVATE)/wrecord.o \
		   $(PRIVATE)/wheader.o	 $(PRIVATE)/wanvl.o	     $(PRIVATE)/wfsmanvl.o

wpylib   = $(PRIVATE)/wfile.o \
		   $(PYTHON)/warc_wrap.o

apylib   = $(PRIVATE)/arecord.o  $(PRIVATE)/afile.o 	 $(PRIVATE)/afsmhdl.o \
		   $(PYTHON)/arc_wrap.o

pyshared_unix: python_clean pyshared
		$(CC) -shared -Wl,-soname$(WPYSONAME) -o $(APPYTHON)/$(WPYSHAREDNAME) \
			   $(pylib) $(wpylib)
		$(CC) -shared -Wl,-soname$(APYSONAME) -o $(APPYTHON)/$(APYSHAREDNAME) \
			   $(pylib) $(apylib)

pyshared_solaris: python_clean pyshared
		$(LD) $(SWIG_LDFLAGS) $(pylib) $(wpylib) -o $(APPYTHON)/$(WPYSHAREDNAME)
		$(CC) $(SWIG_LDFLAGS) $(pylib) $(apylib) -o $(APPYTHON)/$(APYSHAREDNAME)

pyshared_osx: python_clean pyshared
		$(CC) $(SWIG_CFLAGS) $(pylib) $(wpylib) -o $(APPYTHON)/$(WPYSHAREDNAME)
		$(CC) $(SWIG_CFLAGS) $(pylib) $(apylib) -o $(APPYTHON)/$(APYSHAREDNAME) 


pyshared_mingw: python_clean pyshared
		$(CC) -shared -o $(APPYTHON)/$(WPYSHAREDNAME) $(pylib) $(wpylib) \
		-Wl,--out-implib,$(WPYNAME)dll.a
		$(CC) -shared -o $(APPYTHON)/$(APYSHAREDNAME) $(pylib) $(apylib) \
		-Wl,--out-implib,$(APYNAME)dll.a


pyshared_cygwin: python_clean pyshared
		$(CC) -shared $(pylib) $(wpylib) $(LIB_PYTHON)/config -lpython$(PY_VERSION) -o $(APPYTHON)/$(WPYSHAREDNAME)
		$(CC) -shared $(pylib) $(apylib) $(LIB_PYTHON)/config -lpython$(PY_VERSION) -o $(APPYTHON)/$(APYSHAREDNAME)

python:
		@$(MAKE) W_PYSHARED=on $(PYSHARED_OS)

pyshared : $(APPYTHON)/warc.py $(APPYTHON)/arc.py $(pylib) $(wpylib) $(apylib)

$(APPYTHON)/warc.py $(PYTHON)/warc_wrap.c : 
	$(SWIG) -python -outdir $(APPYTHON) $(PYTHON)/warc.i

$(PYTHON)/warc_wrap.o : $(PYTHON)/warc_wrap.c
	$(CC) $(INC_PYTHON) -c $(PYTHON)/warc_wrap.c -o $(PYTHON)/warc_wrap.o
#	$(CC) $(CFLAGS) $(INC_PYTHON) -c $(PYTHON)/warc_wrap.c -o $(PYTHON)/warc_wrap.o

$(APPYTHON)/arc.py $(PYTHON)/arc_wrap.c :
	$(SWIG) -python -outdir $(APPYTHON) $(PYTHON)/arc.i

$(PYTHON)/arc_wrap.o : $(PYTHON)/arc_wrap.c
	$(CC) $(INC_PYTHON) -c $(PYTHON)/arc_wrap.c -o $(PYTHON)/arc_wrap.o
#	$(CC) $(CFLAGS) $(INC_PYTHON) -c $(PYTHON)/arc_wrap.c -o $(PYTHON)/arc_wrap.o


####################
# applications deps
####################

warcdump  = $(PRIVATE)/wclass.o     $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/wanvl.o       $(PRIVATE)/wrecord.o $(PRIVATE)/wheader.o \
		   $(PRIVATE)/wfile.o       $(PRIVATE)/wgetopt.o $(PRIVATE)/wfsmanvl.o \
           ${CSAFE}.o               $(APP)/warcdump.o    $(gzlib) \
           ${MKTEMP}.o  

arc2warc = $(PRIVATE)/wclass.o      $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	       $(PRIVATE)/wanvl.o       $(PRIVATE)/wrecord.o $(PRIVATE)/wheader.o \
	       $(PRIVATE)/wfile.o       $(PRIVATE)/afile.o   $(PRIVATE)/afsmhdl.o \
           $(PRIVATE)/wfsmanvl.o    ${CSAFE}.o           $(PRIVATE)/arecord.o \
           ${MKTEMP}.o              $(TIGER)/tiger.o     $(PRIVATE)/wuuid.o \
		   $(PRIVATE)/wgetopt.o     $(APP)/arc2warc.o    $(gzlib) 

warcfilter  = $(PRIVATE)/wclass.o   $(PRIVATE)/wstring.o  $(PRIVATE)/wlist.o \
		   $(PRIVATE)/wanvl.o       $(PRIVATE)/wrecord.o  $(PRIVATE)/wheader.o \
		   $(PRIVATE)/wfile.o       $(PRIVATE)/wgetopt.o  ${MKTEMP}.o \
           $(PRIVATE)/wfsmanvl.o    ${CSAFE}.o            $(gzlib) \
           $(APP)/warcfilter.o      $(PRIVATE)/wregexp.o  $(regex)

warcvalidator = $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/wanvl.o       $(PRIVATE)/wrecord.o $(PRIVATE)/wheader.o \
		   $(PRIVATE)/wfile.o       $(PRIVATE)/wgetopt.o $(PRIVATE)/wfsmanvl.o \
           ${MKTEMP}.o              ${CSAFE}.o           $(gzlib)  \
           $(APP)/warcvalidator.o    

warcserver = $(PRIVATE)/wclass.o	$(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/wanvl.o 	    $(PRIVATE)/wrecord.o $(PRIVATE)/wheader.o \
	       $(PRIVATE)/wfile.o	    $(PRIVATE)/wuuid.o   $(PRIVATE)/wfsmanvl.o \
		   ${MKTEMP}.o           	${CSAFE}.o	         $(gzlib) \
		   $(PRIVATE)/wserver.o     $(PRIVATE)/wgetopt.o $(TIGER)/tiger.o \
		   $(APP)/warcserver.o	    $(evlib)             $(PRIVATE)/wregexp.o \
           $(regex)

warcclient = $(PRIVATE)/wclass.o	$(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/wanvl.o 	    $(PRIVATE)/wrecord.o $(PRIVATE)/wheader.o\
	       $(PRIVATE)/wfile.o	    $(PRIVATE)/wuuid.o   $(PRIVATE)/wfsmanvl.o \
		   ${MKTEMP}.o	            ${CSAFE}.o	         $(gzlib) \
		   $(PRIVATE)/wclient.o     $(PRIVATE)/wgetopt.o $(TIGER)/tiger.o \
		   $(APP)/warcclient.o	    $(evlib)


####################
# applications 
####################

$(APP)/warcdump:      $(warcdump);       $(CC) $(CFLAGS) -o $@ $(warcdump)
$(APP)/arc2warc:      $(arc2warc);       $(CC) $(CFLAGS) -o $@ $(arc2warc)
$(APP)/warcfilter:    $(warcfilter);     $(CC) $(CFLAGS) -o $@ $(warcfilter)
$(APP)/warcvalidator: $(warcvalidator);  $(CC) $(CFLAGS) -o $@ $(warcvalidator)
$(APP)/warcserver: 	  $(warcserver);  	 $(CC) $(RFLAGS) -o $@ $(warcserver) \
				      $(EV_LIB)
$(APP)/warcclient: 	  $(warcclient);  	 $(CC) $(CFLAGS) -o $@ $(warcclient) \
					  $(EV_LIB)


##############
# freshing
##############

tclean:		;   @rm -f compress* uncompress* *.core out*.warc.gz *.warc.gz *.warc *.xml *.html *.txt *.json

mod_apache_clean:	; @rm -rf $(mod_apache).la $(mod_apache).lo \
					  $(mod_apache).o $(mod_apache).slo $(mod_apache).so \
					  $(APACHE)/.libs $(APACHE)/*~

mod_lighty_clean: ; @rm -rf $(LIGHTTPD)/*.o $(LIGHTTPD)/*~ \
				    $(LIGHTTPD)/warc.cgi $(LIGHTTPD)/warc.fcgi

python_clean: 	   ; @rm -f $(PYTHON)/*.c $(PYTHON)/*.o      
					 rm -f $(APPYTHON)/*.so $(APPYTHON)/warc.py \
					 $(APPYTHON)/arc.py $(APPYTHON)/*.pyc \
					 $(APPYTHON)/*$(LIBSUFFIX)* $(APPYTHON)/_*

clean:		tclean	mod_apache_clean mod_lighty_clean python_clean
			@rm -f $t             $(obj)            *.o \
			       *~             *.a               *.so* \
			       *.log          *.gz              $(PUBLIC)/*~ \
			       $(PLUGIN)/*~   $(GZIP)/*~        $(APP)/*~ \
				   $(APP)/*.exe   $(TST)/*~         $(TST)/*.exe \
                   $(DOC)/*~      $(MINGW_DEP)/*~   $(TIGER)/*~ \
			       $(MISC)/*~     $(MISC)/DEBIAN/*~ $(PRIVATE)/*~ \
				   semantic.cache depend            *.dylib* \
				   *.bak		  *.stackdump*		*core*	\
				   $(EVENT)/*~	  $(EVENT)/*.o      $(MENU)/*~	\
                   $(OUT)/*.xml   $(OUT)/*~		    $(CUNIT)/*~ \
                   $(CUNIT)/*.o   $(REGEX)/*~       $(REGEX)/*.o \
				   ${OSDEP}/*.o   ${OSDEP}/*~       ${MINGW_DEP}/*.o
			@rm -rf $(DOC)/html    warc-tools*

.PHONY: all static clean tclean doc source tgz rpm deb mod_apache_clean mod_lighty_clean
