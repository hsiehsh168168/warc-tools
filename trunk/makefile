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
GZIP     = $(PLUGIN)/gzip
TIGER    = $(PLUGIN)/tiger
EVENT    = $(PLUGIN)/event
EVENT_COMPACT = $(EVENT)/compat
OSDEP    = $(PRIVATE)/os
WIN32DEP = $(OSDEP)/win32
HEADERS  = -I. -I$(PRIVATE) -I$(PUBLIC) -I$(GZIP) -I$(TIGER) \
		   -I$(EVENT) -I$(EVENT_COMPACT)

MAJOR     = 0
MINOR     = 17
RELEASE   = 90
LIBSUFFIX = so
LIBNAME   = libwarc
SONAME	  =,$(LIBNAME).$(LIBSUFFIX).$(MAJOR)
SHAREDNAME=$(LIBNAME).$(LIBSUFFIX).$(MAJOR).$(MINOR).$(RELEASE)

NAME     = warc-tools
PROJECT  = $(NAME)-$(MAJOR).$(MINOR)
PREFIX	 = /usr/local
DESTDIR	 = $(PROJECT)$(PREFIX)/$(NAME)


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


###############
# flags
###############

MAKE	  = make
RANLIB	  = ranlib

GCC       = gcc
CC	  	  = $(GCC) $(HEADERS)

DFLAG     = -g

CFLAGS    = -Wall -W -Wunused -ansi -pedantic -Werror -Wno-long-long \
			-Wunused-function -std=gnu89 $(OPT)

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
MKTEMP = $(PRIVATE)/wmktmp

# compile WARC as a shared library
SHARED_OS = shared_unix
ifeq ($(W_SHARED),on)
	S_CFLAGS = -fPIC -DPIC
endif

# adapt makefile to OS
ifeq ($(UNAME_S),Linux)
	CFLAGS      += -pedantic-errors
	EV_OS   	 = $(EVENT)/os/linux
	HEADERS     += -I$(EV_OS)
	EV_SRC		 = $(EVENT)/epoll.c $(EVENT)/poll.c $(EVENT)/strlcpy.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = -lrt 
#-lnsl -lresolv
endif
ifeq ($(UNAME_S),FreeBSD)
	MAKE         = gmake
	EV_OS   	 = $(EVENT)/os/freebsd
	HEADERS    += -I$(EV_OS)
	EV_SRC		 = $(EVENT)/kqueue.c $(EVENT)/poll.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = 
endif
ifeq ($(UNAME_S),OpenBSD)
	MAKE         = gmake
	CFLAGS      += -DWINLINE=""
	EV_OS        = $(EVENT)/os/openbsd
	HEADERS     += -I$(EV_OS)
	EV_SRC		 = $(EVENT)/kqueue.c $(EVENT)/poll.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = 
	GCC_EXTRA    = 
endif
ifeq ($(UNAME_S),NetBSD)
	MAKE         = gmake
	EV_OS        = $(EVENT)/os/netbsd
	HEADERS     += -I$(EV_OS)
	EV_SRC		 = $(EVENT)/kqueue.c $(EVENT)/poll.c
	EVENT_CONFIG = $(EVENT)/config.h $(EVENT)/event-config.h
	EV_LIB		 = 
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS 		+= -pedantic
	EV_OS        = $(EVENT)/os/osx
	HEADERS     += -I$(EV_OS)
	EV_SRC		 = $(EVENT)/kqueue.c $(EVENT)/poll.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = 
#-lresolv
	SHARED_OS	 = shared_osx
	LIBSUFFIX    = dylib
	INSTALLNAME	 = $(LIBNAME).$(MAJOR).$(LIBSUFFIX)
	SHAREDNAME	 = $(LIBNAME).$(MAJOR).$(MINOR).$(RELEASE).$(LIBSUFFIX)
	S_CFLAGS     =
endif
ifeq ($(UNAME_S),SunOS)
	EV_OS        = $(EVENT)/os/solaris
	HEADERS     += -I$(EV_OS)
	EV_SRC		 = $(EVENT)/devpoll.c $(EVENT)/poll.c $(EVENT)/evport.c $(EVENT)/strlcpy.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = -lrt -lsocket
#-lnsl -lresolv -lsocket
	SONAME	     =
endif
ifneq ($(findstring MINGW,$(UNAME_S)),)
	MKTEMP     = $(WIN32DEP)/wmktmp
	HEADERS   := $(HEADERS) -I$(WIN32DEP)
	SHARED_OS  = shared_mingw
	LIBSUFFIX  = dll
	SHAREDNAME = $(LIBNAME).$(LIBSUFFIX)
	S_CFLAGS   = -DBUILD_DLL
	BASH	   = sh
endif
ifneq ($(findstring CYGWIN,$(UNAME_S)),)
	EV_OS        = $(EVENT)/os/cygwin
	HEADERS     := $(HEADERS) -I$(WIN32DEP) -I$(EV_OS)
	EV_SRC		 = $(EVENT)/poll.c
	EVENT_CONFIG = $(EV_OS)/config.h $(EV_OS)/event-config.h
	EV_LIB		 = -lrt
#-lnsl -lresolv
	SHARED_OS    = shared_cygwin
	LIBSUFFIX    = dll
	SHAREDNAME   = cyg$(LIBNAME).$(LIBSUFFIX)
	S_CFLAGS     = 
endif


# post OS flags processing

CFLAGS += $(GCC_EXTRA)
CFLAGS += $(S_CFLAGS)


###############
# sources
###############

b	= $(PRIVATE)/wstring.c   $(PRIVATE)/wclass.c  $(PRIVATE)/wlist.c \
	  $(PRIVATE)/whdline.c   $(PRIVATE)/wfile.c   $(PRIVATE)/wrecord.c \
	  $(PRIVATE)/wanvl.c     $(PRIVATE)/wfsmhdl.c $(PRIVATE)/afsmhdl.c \
      $(PRIVATE)/wfsmanvl.c  $(PRIVATE)/wcsafe.c  $(PRIVATE)/wgetopt.c \
      $(PRIVATE)/arecord.c   $(PRIVATE)/afile.c   ${MKTEMP}.c \
      $(PRIVATE)/wendian.c   $(PRIVATE)/wuuid.c   $(PRIVATE)/wserver.c \
	  $(PRIVATE)/whash.c     $(PRIVATE)/wkv.c	  $(PRIVATE)/wgzip.c \
	  $(PRIVATE)/wclient.c

b	+= $(GZIP)/adler32.c     $(GZIP)/crc32.c      $(GZIP)/deflate.c \
	  $(GZIP)/infback.c      $(GZIP)/inffast.c    $(GZIP)/inflate.c \
	  $(GZIP)/inftrees.c     $(GZIP)/uncompr.c    $(GZIP)/wgzipbit.c \
	  $(GZIP)/zutil.c        $(GZIP)/compress.c   $(GZIP)/trees.c

b	+= $(TIGER)/tiger.c

EV_SRC += \
	  $(EVENT)/event.c		 $(EVENT)/buffer.c    $(EVENT)/log.c \
	  $(EVENT)/evutil.c      $(EVENT)/http.c	  $(EVENT)/signal.c \
	  $(EVENT)/select.c		 $(EVENT)/min_heap.c
#$(EVENT)/evbuffer.c $(EVENT)/event_tagging.c $(EVENT)/evrpc.c
b	+=$(EV_SRC)

c	= $(b) \
	  $(TST)/string.c        $(TST)/list.c        $(TST)/hdline.c \
	  $(TST)/anvl.c          $(TST)/record.c      $(TST)/file.c  \
	  $(TST)/arcrecord.c     $(TST)/warcgzip.c    $(TST)/warcgunzip.c \
	  $(TST)/arcfile.c       $(TST)/a2w.c         $(TST)/uuid.c \
	  $(TST)/getopt.c        $(TST)/object.c      $(TST)/hash.c \
	  $(TST)/server.c		 $(TST)/client.c

c	+= $(APP)/arc2warc.c     $(APP)/warcdump.c    $(APP)/warcfilter.c \
	  $(APP)/warcvalidator.c $(APP)/warcserver.c  $(APP)/warcclient.c

h	= $(PUBLIC)/wclass.h     $(PUBLIC)/warc.h     $(PRIVATE)/wstring.h \
	  $(PUBLIC)/wrtype.h     $(PUBLIC)/wfile.h    $(PRIVATE)/wlist.h \
	  $(PRIVATE)/whdline.h   $(PRIVATE)/wuuid.h   $(PRIVATE)/wgetopt.h \
	  $(PUBLIC)/wrecord.h    $(PRIVATE)/wanvl.h   $(PRIVATE)/wmem.h \
	  $(PRIVATE)/fsm.h       $(PRIVATE)/wfsmhdl.h $(PRIVATE)/fsmanvl.h \
	  $(PRIVATE)/wcsafe.h    $(PRIVATE)/afsmhdl.h $(PRIVATE)/arecord.h \
	  $(PRIVATE)/afile.h     $(PRIVATE)/wmktmp.h  $(PRIVATE)/arecord.h \
	  $(PRIVATE)/wendian.h   $(PRIVATE)/whash.h   $(PRIVATE)/wkv.h \
	  $(PUBLIC)/wgzip.h		 $(PRIVATE)/wserver.h $(PRIVATE)/wclient.h

h   += $(GZIP)/crc32.h       $(GZIP)/deflate.h    $(GZIP)/inffast.h \
	  $(GZIP)/inffixed.h     $(GZIP)/inflate.h    $(GZIP)/inftrees.h \
	  $(GZIP)/wgzipbit.h     $(GZIP)/wos.h        $(GZIP)/zconf.h \
	  $(GZIP)/zlib.h         $(GZIP)/zutil.h      $(GZIP)/trees.h
h	+= $(TIGER)/tiger.h
h   +=$(EVENT)/log.h	  $(EVENT)/event-internal.h	  $(EVENT)/evdns.h \
	  $(EVENT)/event.h	  $(EVENT)/event-config.h	  $(EVENT)/evrpc.h \
	  $(EVENT)/min_heap.h $(EVENT)/strlcpy-internal.h $(EVENT_CONFIG) \
	  $(EVENT)/evutil.h	  $(EVENT)/evsignal.h        $(EVENT)/http-internal.h \
	  $(EVENT)/evhttp.h	  $(EVENT)/evrpc-internal.h \
	  $(EVENT)/compat/sys/queue.h $(EVENT)/compat/sys/_time.h

u	= $(TST)/string          $(TST)/list          $(TST)/anvl \
	  $(TST)/record          $(TST)/uuid          $(TST)/hdline \
	  $(TST)/warcgzip        $(TST)/warcgunzip    $(TST)/file \
	  $(TST)/arcrecord       $(TST)/arcfile       $(TST)/a2w \
	  $(TST)/getopt          $(TST)/hash		  $(TST)/server \
	  $(TST)/client			 $(TST)/object

t  += $(u)

a  = $(APP)/arc2warc         $(APP)/warcdump      $(APP)/warcfilter \
	 $(APP)/warcvalidator	 $(APP)/warcserver	  $(APP)/warcclient

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
evlib   = $(EV_SRC:.c=.o)

all:  	$t

static:	$(libwarc)	; ar cvr $(LIBNAME).a $(libwarc); $(RANLIB) $(LIBNAME).a

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

source:	shared static $(a)
		rm -rf $(PROJECT)
		mkdir -p $(DESTDIR)/bin
		mkdir -p $(DESTDIR)/include/compat/sys
		mkdir -p $(DESTDIR)/lib
		cp -f readme $(DESTDIR)
		cp -f license/apache2-license $(DESTDIR)
		cp -f $(a) $(DESTDIR)/bin
		cp -f $(APP)/*.sh $(DESTDIR)/bin
		find $(LIB) -name "*.h" -type "f" -exec cp -f '{}' $(DESTDIR)/include \;
		find $(LIB) -name "*.x" -type "f" -exec cp -f '{}' $(DESTDIR)/include \;
		cp -f $(EVENT_CONFIG) $(DESTDIR)/include
		mv $(DESTDIR)/include/_time.h $(DESTDIR)/include/compat/sys
		mv $(DESTDIR)/include/queue.h $(DESTDIR)/include/compat/sys
		cp -rf $(APACHE) $(DESTDIR)
		mv $(LIBNAME).a              $(DESTDIR)/lib
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


mod_apache_install: mod_apache
		@echo
		@echo "------------------------------------------------------------"
		@echo "Install \"mod_warc\" for Apache2 web server."
		@echo "YOU MUST BE \"root\"."
		@echo "------------------------------------------------------------"
		$(APXS) -i -a $(mod_apache).la
mod_apache:mod_apache_clean install
		$(APXS) -I$(PREFIX)/$(NAME)/include -L$(PREFIX)/$(NAME)/lib \
		-c $(mod_apache).c 




test_all:	;	@$(BASH) $(TST)/test.sh $(u)
test:   $(u) test_all tclean

doc:        ;   doxygen ./doc/warcdoc

.c.o:
	  	${CC} ${CFLAGS} -o $@ -c $<



##################
# unit tests deps
##################

string	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(TST)/string.o \
		  $(PRIVATE)/wcsafe.o

list	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  $(PRIVATE)/wcsafe.o    $(TST)/list.o

hdline	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/whdline.o \
		  $(PRIVATE)/wfsmhdl.o   $(PRIVATE)/wcsafe.o    $(TST)/hdline.o

anvl	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wanvl.o \
		  $(TST)/anvl.o          $(PRIVATE)/wfsmanvl.o  $(PRIVATE)/wcsafe.o

record	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  $(PRIVATE)/whdline.o   $(PRIVATE)/wanvl.o     $(PRIVATE)/wrecord.o \
	      $(PRIVATE)/wfsmhdl.o   $(PRIVATE)/wfsmanvl.o  $(PRIVATE)/wcsafe.o \
		  ${MKTEMP}.o            $(TST)/record.o

warcgzip  = $(PRIVATE)/wclass.o  $(gzlib)               $(TST)/warcgzip.o \
		  $(PRIVATE)/wcsafe.o

warcgunzip= $(PRIVATE)/wclass.o  $(gzlib)               $(TST)/warcgunzip.o \
		  $(PRIVATE)/wcsafe.o

object	= $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  $(PRIVATE)/wanvl.o     $(TST)/object.o        $(PRIVATE)/wcsafe.o 

file    = $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
	  	  $(PRIVATE)/whdline.o   $(PRIVATE)/wanvl.o     $(PRIVATE)/wrecord.o \
	  	  $(PRIVATE)/wfile.o     $(PRIVATE)/wfsmhdl.o   $(PRIVATE)/wuuid.o \
          $(PRIVATE)/wfsmanvl.o  $(PRIVATE)/wcsafe.o    $(gzlib) \
		  ${MKTEMP}.o            $(TST)/file.o          $(TIGER)/tiger.o

arcrecord = $(PRIVATE)/arecord.o $(PRIVATE)/wstring.o   $(PRIVATE)/wclass.o \
          $(PRIVATE)/afsmhdl.o   $(PRIVATE)/wcsafe.o    $(PRIVATE)/wrecord.o \
          $(PRIVATE)/whdline.o   $(PRIVATE)/wanvl.o     $(PRIVATE)/wlist.o \
          $(PRIVATE)/afile.o     ${MKTEMP}.o            ${gzlib} \
          $(TST)/arcrecord.o

a2w    = $(PRIVATE)/wclass.o     $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
	 	 $(PRIVATE)/whdline.o    $(PRIVATE)/wanvl.o     $(PRIVATE)/wrecord.o \
	 	 $(PRIVATE)/wfile.o      $(PRIVATE)/wfsmhdl.o   $(PRIVATE)/afsmhdl.o \
	 	 $(PRIVATE)/wfsmanvl.o   $(PRIVATE)/wcsafe.o    $(PRIVATE)/arecord.o \
	 	 $(PRIVATE)/afile.o      $(gzlib)               ${MKTEMP}.o \
	 	 $(TST)/a2w.o              

arcfile = $(PRIVATE)/wclass.o    $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
	  	 $(PRIVATE)/wcsafe.o     $(PRIVATE)/whdline.o   $(PRIVATE)/wanvl.o \
         $(PRIVATE)/wrecord.o    $(PRIVATE)/arecord.o   $(PRIVATE)/afsmhdl.o \
		 $(PRIVATE)/afile.o      $(TST)/arcfile.o       ${MKTEMP}.o \
	     $(gzlib)

uuid	= $(PRIVATE)/wclass.o    $(PRIVATE)/wuuid.o     $(PRIVATE)/wcsafe.o \
		 $(PRIVATE)/wstring.o    $(TIGER)/tiger.o       $(TST)/uuid.o

getopt	= $(PRIVATE)/wclass.o    $(PRIVATE)/wgetopt.o   $(PRIVATE)/wcsafe.o \
		 $(PRIVATE)/wstring.o    $(TST)/getopt.o

hash	= $(PRIVATE)/wclass.o    $(PRIVATE)/whash.o   	$(PRIVATE)/wcsafe.o \
		  $(PRIVATE)/wlist.o	 $(PRIVATE)/wstring.o   $(PRIVATE)/wkv.o \
		  $(TST)/hash.o

server =  $(PRIVATE)/wclass.o	 $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  $(PRIVATE)/whdline.o 	 $(PRIVATE)/wanvl.o 	$(PRIVATE)/wrecord.o \
	      $(PRIVATE)/wfile.o	 $(PRIVATE)/wfsmhdl.o   $(PRIVATE)/wuuid.o \
		  $(PRIVATE)/wfsmanvl.o	 $(PRIVATE)/wcsafe.o    $(gzlib) \
		  ${MKTEMP}.o 			 $(PRIVATE)/wserver.o   $(TIGER)/tiger.o \
		  $(TST)/server.o		 $(evlib)

client =  $(PRIVATE)/wclass.o	 $(PRIVATE)/wstring.o   $(PRIVATE)/wlist.o \
		  $(PRIVATE)/whdline.o 	 $(PRIVATE)/wanvl.o 	$(PRIVATE)/wrecord.o \
	      $(PRIVATE)/wfile.o	 $(PRIVATE)/wfsmhdl.o   $(PRIVATE)/wuuid.o \
		  $(PRIVATE)/wfsmanvl.o	 $(PRIVATE)/wcsafe.o    $(gzlib) \
		  ${MKTEMP}.o 			 $(PRIVATE)/wclient.o   $(TIGER)/tiger.o \
		  $(TST)/client.o		 $(evlib)



##################
# unit tests deps
##################

$(TST)/string:	  $(string);     $(CC) $(CFLAGS)   -o $@ $(string)
$(TST)/list:	  $(list);       $(CC) $(CFLAGS)   -o $@ $(list)
$(TST)/hdline:	  $(hdline);     $(CC) $(CFLAGS)   -o $@ $(hdline)
$(TST)/record:	  $(record);     $(CC) $(CFLAGS)   -o $@ $(record)
$(TST)/warcgzip:  $(warcgzip);   $(CC) $(CFLAGS)   -o $@ $(warcgzip)
$(TST)/warcgunzip:$(warcgunzip); $(CC) $(CFLAGS)   -o $@ $(warcgunzip)
$(TST)/anvl:	  $(anvl);       $(CC) $(CFLAGS)   -o $@ $(anvl)
$(TST)/object:	  $(object);     $(CC) $(CFLAGS)   -o $@ $(object)
$(TST)/file:      $(file);       $(CC) $(CFLAGS)   -o $@ $(file)
$(TST)/arcrecord: $(arcrecord);  $(CC) $(CFLAGS)   -o $@ $(arcrecord)
$(TST)/a2w:       $(a2w);        $(CC) $(CFLAGS)   -o $@ $(a2w)
$(TST)/arcfile:   $(arcfile);    $(CC) $(CFLAGS)   -o $@ $(arcfile)
$(TST)/uuid:	  $(uuid);       $(CC) $(CFLAGS)   -o $@ $(uuid)
$(TST)/getopt:	  $(getopt);     $(CC) $(CFLAGS)   -o $@ $(getopt)
$(TST)/hash:	  $(hash);       $(CC) $(CFLAGS)   -o $@ $(hash)
$(TST)/server:	  $(server);	 ${CC} ${CFLAGS}   -o $@ $(server) $(EV_LIB)
$(TST)/client:	  $(client);	 ${CC} ${CFLAGS}   -o $@ $(client) $(EV_LIB)


####################
# applications deps
####################

warcdump  = $(PRIVATE)/wclass.o     $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/whdline.o     $(PRIVATE)/wanvl.o   $(PRIVATE)/wrecord.o \
		   $(PRIVATE)/wfile.o       $(PRIVATE)/wfsmhdl.o $(PRIVATE)/wgetopt.o \
           $(PRIVATE)/wfsmanvl.o    $(PRIVATE)/wcsafe.o  $(gzlib) \
           $(APP)/warcdump.o        ${MKTEMP}.o  

arc2warc = $(PRIVATE)/wclass.o      $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	       $(PRIVATE)/whdline.o     $(PRIVATE)/wanvl.o   $(PRIVATE)/wrecord.o \
	       $(PRIVATE)/wfile.o       $(PRIVATE)/wfsmhdl.o $(PRIVATE)/afile.o \
           $(PRIVATE)/wfsmanvl.o    $(PRIVATE)/wcsafe.o  $(PRIVATE)/arecord.o \
           $(PRIVATE)/afsmhdl.o     $(TIGER)/tiger.o     $(PRIVATE)/wuuid.o \
		   $(APP)/arc2warc.o        ${MKTEMP}.o          $(gzlib) \
		   $(PRIVATE)/wgetopt.o

warcfilter  = $(PRIVATE)/wclass.o   $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/whdline.o     $(PRIVATE)/wanvl.o   $(PRIVATE)/wrecord.o \
		   $(PRIVATE)/wfile.o       $(PRIVATE)/wfsmhdl.o $(PRIVATE)/wgetopt.o \
           $(PRIVATE)/wfsmanvl.o    $(PRIVATE)/wcsafe.o  $(gzlib) \
           $(APP)/warcfilter.o      ${MKTEMP}.o  

warcvalidator = $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/whdline.o     $(PRIVATE)/wanvl.o   $(PRIVATE)/wrecord.o \
		   $(PRIVATE)/wfile.o       $(PRIVATE)/wfsmhdl.o $(PRIVATE)/wgetopt.o \
           $(PRIVATE)/wfsmanvl.o    $(PRIVATE)/wcsafe.o  $(gzlib)  \
           $(APP)/warcvalidator.o   ${MKTEMP}.o 

warcserver = $(PRIVATE)/wclass.o	$(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/whdline.o 	$(PRIVATE)/wanvl.o 	 $(PRIVATE)/wrecord.o \
	       $(PRIVATE)/wfile.o	    $(PRIVATE)/wfsmhdl.o $(PRIVATE)/wuuid.o \
		   $(PRIVATE)/wfsmanvl.o	$(PRIVATE)/wcsafe.o	 $(gzlib) \
		   ${MKTEMP}.o			    $(PRIVATE)/wserver.o $(PRIVATE)/wgetopt.o \
		   $(TIGER)/tiger.o			$(APP)/warcserver.o	 $(evlib)

warcclient = $(PRIVATE)/wclass.o	$(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/whdline.o 	$(PRIVATE)/wanvl.o 	 $(PRIVATE)/wrecord.o \
	       $(PRIVATE)/wfile.o	    $(PRIVATE)/wfsmhdl.o $(PRIVATE)/wuuid.o \
		   $(PRIVATE)/wfsmanvl.o	$(PRIVATE)/wcsafe.o	 $(gzlib) \
		   ${MKTEMP}.o			    $(PRIVATE)/wclient.o $(PRIVATE)/wgetopt.o \
		   $(TIGER)/tiger.o			$(APP)/warcclient.o	 $(evlib)


####################
# applications 
####################

$(APP)/warcdump:      $(warcdump);       $(CC) $(CFLAGS) -o $@ $(warcdump)
$(APP)/arc2warc:      $(arc2warc);       $(CC) $(CFLAGS) -o $@ $(arc2warc)
$(APP)/warcfilter:    $(warcfilter);     $(CC) $(CFLAGS) -o $@ $(warcfilter)
$(APP)/warcvalidator: $(warcvalidator);  $(CC) $(CFLAGS) -o $@ $(warcvalidator)
$(APP)/warcserver: 	  $(warcserver);  	 $(CC) $(CFLAGS) -o $@ $(warcserver) \
				      $(EV_LIB)
$(APP)/warcclient: 	  $(warcclient);  	 $(CC) $(CFLAGS) -o $@ $(warcclient) \
					  $(EV_LIB)


##############
# freshing
##############

tclean:		;   @rm -f compress* uncompress* *.core out*.warc.gz *.warc.gz *.warc

mod_apache_clean:	; @rm -rf $(mod_apache).la $(mod_apache).lo \
					  $(mod_apache).o $(mod_apache).slo $(mod_apache).so \
					  $(APACHE)/.libs $(APACHE)/*~


clean:		tclean	mod_apache_clean
			@rm -f $t             $(obj)            *.o \
			       *~             *.a               *.so* \
			       *.log          *.gz              $(PUBLIC)/*~ \
			       $(PLUGIN)/*~   $(GZIP)/*~        $(APP)/*~ \
				   $(APP)/*.exe   $(TST)/*~         $(TST)/*.exe \
                   $(DOC)/*~      $(WIN32DEP)/*~    $(TIGER)/*~ \
			       $(MISC)/*~     $(MISC)/DEBIAN/*~ $(PRIVATE)/*~ \
				   semantic.cache depend            *.dylib* \
				   *.bak		  *.stackdump*		*core*	\
				   $(EVENT)/*~	  $(EVENT)/*.o
			@rm -rf $(DOC)/html    warc-tools*

.PHONY: all static clean tclean doc source tgz rpm deb mod_apache_clean
