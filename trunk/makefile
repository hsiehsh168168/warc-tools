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
NAME     = warc-tools
PROJECT  = $(NAME)-$(VERSION)

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
OSDEP    = $(PRIVATE)/os
WIN32DEP = $(OSDEP)/win32
HEADERS  = -I. -I$(PRIVATE) -I$(PUBLIC) -I$(GZIP) -I$(TIGER)

###############
# flags
###############

GCC       = gcc
CC	  	  = $(GCC) $(HEADERS)

DFLAG     = -g

CFLAGS    = -Wall -W -Wunused -ansi -pedantic -Werror -Wno-long-long \
			-Wunused-function -std=gnu89 $(OPT)
CFLAGS += $(DFLAG)
#CFLAGS  += -Wunreachable-code 
#CFLAGS  += -NDEBUG
#CFLAGS  += -Wconversion -Wtraditional -Wmissing-prototypes -Wshadow

# uncomment the line below to compile with large file settings (i.e. 64 bits offsets)
CFLAGS  += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES

# uncomment the line below to compile with optimization enabled
#CFLAGS += -O3 -march=i686 -pipe

# uncomment the line below to disable all assertions and traces
#CFLAGS += -DNDEBUG

# uncomment the line below to disable all printing via "w_fprintf"
#CFLAGS += -Dw_fprintf=""

# GCC extra flags
GCC_EXTRA = -Wextra

# OS dependant functions (for portability issue)
MKTEMP = $(PRIVATE)/wmktmp



ifeq ($(UNAME_S),Linux)
	CFLAGS  += -pedantic-errors
endif
ifeq ($(UNAME_S),FreeBSD)
endif
ifeq ($(UNAME_S),OpenBSD)
	CFLAGS    += -DWINLINE=""
	GCC_EXTRA  = 
endif
ifeq ($(UNAME_S),NetBSD)
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -pedantic
endif
ifeq ($(UNAME_S),SunOS)
endif
ifneq ($(findstring MINGW,$(UNAME_S)),)
	MKTEMP   = $(WIN32DEP)/wmktmp32
	HEADERS := $(HEADERS) -I$(WIN32DEP)
endif
ifneq ($(findstring CYGWIN,$(UNAME_S)),)
	MKTEMP = $(WIN32DEP)/wmktmp32
	HEADERS := $(HEADERS) -I$(WIN32DEP)
endif


CFLAGS += $(GCC_EXTRA)

###############
# sources
###############

b	= $(PRIVATE)/wstring.c $(PRIVATE)/wclass.c $(PRIVATE)/wlist.c \
	  $(PRIVATE)/whdline.c $(PRIVATE)/wfile.c $(PRIVATE)/wrecord.c \
	  $(PRIVATE)/wanvl.c $(PRIVATE)/wfsmhdl.c $(PRIVATE)/afsmhdl.c \
      $(PRIVATE)/wfsmanvl.c $(PRIVATE)/wcsafe.c \
      $(PRIVATE)/arecord.c  $(PRIVATE)/afile.c ${MKTEMP}.c \
      $(PRIVATE)/wendian.c $(PRIVATE)/wuuid.c $(PRIVATE)/wgetopt.c
b     += $(GZIP)/adler32.c $(GZIP)/crc32.c $(GZIP)/deflate.c \
	  $(GZIP)/infback.c $(GZIP)/inffast.c $(GZIP)/inflate.c \
	  $(GZIP)/inftrees.c $(GZIP)/uncompr.c $(GZIP)/wgzipbit.c \
	  $(GZIP)/zutil.c $(GZIP)/compress.c $(GZIP)/trees.c \
	  $(PRIVATE)/wgzip.c
b	  += $(TIGER)/tiger.c

c     = $(b) \
	  $(TST)/string.c $(TST)/list.c $(TST)/hdline.c $(TST)/object.c \
	  $(TST)/anvl.c $(TST)/record.c $(TST)/file.c $(TST)/arcrecord.c \
	  $(TST)/gzip.c $(TST)/gunzip.c  $(TST)/arcfile.c  $(TST)/a2w.c \
	  $(TST)/uuid.c $(TST)/getopt.c
c     += $(APP)/arc2warc.c $(APP)/warcdump.c $(APP)/warcfilter.c \
		$(APP)/warcvalidator.c

h	= $(PUBLIC)/wclass.h $(PUBLIC)/warc.h $(PRIVATE)/wstring.h \
	  $(PUBLIC)/wrtype.h $(PUBLIC)/wfile.h \
	  $(PRIVATE)/wlist.h $(PRIVATE)/whdline.h \
	  $(PUBLIC)/wrecord.h $(PRIVATE)/wanvl.h $(PRIVATE)/wmem.h \
	  $(PRIVATE)/fsm.h $(PRIVATE)/wfsmhdl.h $(PRIVATE)/fsmanvl.h \
	  $(PRIVATE)/wcsafe.h $(PRIVATE)/afsmhdl.h $(PRIVATE)/arecord.h \
	  $(PRIVATE)/afile.h $(PRIVATE)/wmktmp.h $(PRIVATE)/arecord.h \
	  $(PRIVATE)/wendian.h $(PRIVATE)/wuuid.h $(PRIVATE)/wgetopt.h
h   += $(GZIP)/crc32.h $(GZIP)/deflate.h $(GZIP)/inffast.h \
	  $(GZIP)/inffixed.h $(GZIP)/inflate.h $(GZIP)/inftrees.h \
	  $(GZIP)/wgzipbit.h $(GZIP)/wos.h $(GZIP)/zconf.h $(GZIP)/zlib.h \
	  $(GZIP)/zutil.h $(GZIP)/trees.h $(PUBLIC)/wgzip.h
h	   += $(TIGER)/tiger.h

u	= $(TST)/string $(TST)/list   $(TST)/anvl \
	  $(TST)/record $(TST)/object $(TST)/hdline \
	  $(TST)/gzip   $(TST)/gunzip $(TST)/file \
	  $(TST)/arcrecord $(TST)/arcfile $(TST)/a2w \
	  $(TST)/uuid $(TST)/getopt
t  += $(u)

a  = $(APP)/arc2warc $(APP)/warcdump $(APP)/warcfilter $(APP)/warcvalidator
t  += $(a)


###############
# library
###############

libwarc = $(b:.c=.o)
obj     = $(libwarc) $(c:.c=.o)
gzlib = $(GZIP)/adler32.o $(GZIP)/crc32.o $(GZIP)/deflate.o \
		$(GZIP)/infback.o $(GZIP)/inffast.o $(GZIP)/inflate.o \
	  	$(GZIP)/inftrees.o $(GZIP)/uncompr.o $(GZIP)/wgzipbit.o \
	  	$(GZIP)/zutil.o $(GZIP)/compress.o $(GZIP)/trees.o \
		$(PRIVATE)/wgzip.o $(PRIVATE)/wendian.o

all:  		$t

static:		$(libwarc)	; ar cvr libwarc.a $(libwarc); ranlib libwarc.a

source:	static $(a)
		rm -rf $(PROJECT)
		mkdir -p $(PROJECT)/usr/local/bin
		mkdir -p $(PROJECT)/usr/local/include
		mkdir -p $(PROJECT)/usr/local/lib
		cp -f $(a) $(PROJECT)/usr/local/bin
		cp -f $(APP)/*.sh $(PROJECT)/usr/local/bin
		find $(LIB) -name "*.h" -type "f" -exec cp -f '{}' $(PROJECT)/usr/local/include \;
		find $(LIB) -name "*.x" -type "f" -exec cp -f '{}' $(PROJECT)/usr/local/include \;
		mv libwarc.a $(PROJECT)/usr/local/lib

tgz:	source
		rm -f $(PROJECT).tar.gz
		tar cvf $(PROJECT).tar warc-tools-$(VERSION)
		gzip -9 $(PROJECT).tar
		rm -rf $(PROJECT)

deb:	source
		rm -f $(PROJECT).deb
		cp -rf $(MISC)/DEBIAN $(PROJECT)/
		dpkg-deb --build  $(PROJECT)
		rm -rf $(PROJECT)

rpm:	deb
		rm -f $(PROJECT).rpm
		alien -r $(PROJECT).deb
		rm -f $(PROJECT).deb

test:	$(u)
		@$(TST)/test.sh $(u)

doc:        ;   doxygen ./doc/warcdoc

.c.o:
	  	${CC} ${CFLAGS} -o $@ -c $<



##################
# unit tests deps
##################

string	= $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(TST)/string.o $(PRIVATE)/wcsafe.o
list	= $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	$(PRIVATE)/wcsafe.o $(TST)/list.o
hdline	= $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/whdline.o \
	  $(PRIVATE)/wfsmhdl.o $(PRIVATE)/wcsafe.o $(TST)/hdline.o
anvl	= $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wanvl.o \
	  $(TST)/anvl.o  $(PRIVATE)/wfsmanvl.o  $(PRIVATE)/wcsafe.o
record	= $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	  $(PRIVATE)/whdline.o $(PRIVATE)/wanvl.o $(PRIVATE)/wrecord.o \
	  $(PRIVATE)/wfsmhdl.o $(PRIVATE)/wfsmanvl.o \
	  $(PRIVATE)/wcsafe.o ${MKTEMP}.o $(TST)/record.o
gzip	= $(PRIVATE)/wclass.o $(gzlib) $(TST)/gzip.o $(PRIVATE)/wcsafe.o
gunzip	= $(PRIVATE)/wclass.o $(gzlib) $(TST)/gunzip.o  $(PRIVATE)/wcsafe.o
object	= $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	  $(PRIVATE)/wanvl.o $(TST)/object.o $(PRIVATE)/wcsafe.o 
file    = $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	  $(PRIVATE)/whdline.o $(PRIVATE)/wanvl.o $(PRIVATE)/wrecord.o \
	  $(PRIVATE)/wfile.o   $(PRIVATE)/wfsmhdl.o $(PRIVATE)/wuuid.o \
          $(PRIVATE)/wfsmanvl.o $(PRIVATE)/wcsafe.o $(gzlib) ${MKTEMP}.o \
          $(TST)/file.o $(TIGER)/tiger.o
arcrecord = $(PRIVATE)/arecord.o $(PRIVATE)/wstring.o $(PRIVATE)/wclass.o \
            $(PRIVATE)/afsmhdl.o $(PRIVATE)/wcsafe.o $(PRIVATE)/wrecord.o \
            $(PRIVATE)/whdline.o $(PRIVATE)/wanvl.o $(PRIVATE)/wlist.o \
            $(PRIVATE)/afile.o \
            ${MKTEMP}.o ${gzlib} \
            $(TST)/arcrecord.o
a2w    = $(PRIVATE)/wclass.o   $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	 $(PRIVATE)/whdline.o  $(PRIVATE)/wanvl.o   $(PRIVATE)/wrecord.o \
	 $(PRIVATE)/wfile.o    $(PRIVATE)/wfsmhdl.o \
	 $(PRIVATE)/wfsmanvl.o $(PRIVATE)/wcsafe.o  $(PRIVATE)/arecord.o \
	 $(PRIVATE)/afsmhdl.o  $(PRIVATE)/afile.o \
	 $(TST)/a2w.o ${MKTEMP}.o $(gzlib) 
arcfile = $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	  $(PRIVATE)/wcsafe.o $(PRIVATE)/whdline.o $(PRIVATE)/wanvl.o \
          $(PRIVATE)/wrecord.o \
	  $(PRIVATE)/arecord.o $(PRIVATE)/afsmhdl.o $(PRIVATE)/afile.o \
	  $(TST)/arcfile.o ${MKTEMP}.o $(gzlib)
uuid	= $(PRIVATE)/wclass.o $(PRIVATE)/wuuid.o $(PRIVATE)/wcsafe.o \
		$(PRIVATE)/wstring.o $(TIGER)/tiger.o $(TST)/uuid.o

getopt	= $(PRIVATE)/wclass.o $(PRIVATE)/wgetopt.o $(PRIVATE)/wcsafe.o \
		$(PRIVATE)/wstring.o $(TST)/getopt.o

##################
# unit tests deps
##################

$(TST)/string:	  $(string);     $(CC) $(CFLAGS)   -o $@ $(string)
$(TST)/list:	  $(list);       $(CC) $(CFLAGS)   -o $@ $(list)
$(TST)/hdline:	  $(hdline);     $(CC) $(CFLAGS)   -o $@ $(hdline)
$(TST)/record:	  $(record);     $(CC) $(CFLAGS)   -o $@ $(record)
$(TST)/gzip:	  $(gzip);       $(CC) $(CFLAGS)   -o $@ $(gzip)
$(TST)/gunzip:	  $(gunzip);     $(CC) $(CFLAGS)   -o $@ $(gunzip)
$(TST)/anvl:	  $(anvl);       $(CC) $(CFLAGS)   -o $@ $(anvl)
$(TST)/object:	  $(object);     $(CC) $(CFLAGS)   -o $@ $(object)
$(TST)/file:      $(file);       $(CC) $(CFLAGS)   -o $@ $(file)
$(TST)/arcrecord: $(arcrecord);  $(CC) $(CFLAGS)   -o $@ $(arcrecord)
$(TST)/a2w:       $(a2w);        $(CC) $(CFLAGS)   -o $@ $(a2w)
$(TST)/arcfile:   $(arcfile);    $(CC) $(CFLAGS)   -o $@ $(arcfile)
$(TST)/uuid:	  $(uuid);       $(CC) $(CFLAGS)   -o $@ $(uuid)
$(TST)/getopt:	  $(getopt);     $(CC) $(CFLAGS)   -o $@ $(getopt)

####################
# applications deps
####################

warcdump  = $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/whdline.o $(PRIVATE)/wanvl.o $(PRIVATE)/wrecord.o \
		   $(PRIVATE)/wfile.o  $(PRIVATE)/wfsmhdl.o \
           $(PRIVATE)/wfsmanvl.o $(PRIVATE)/wcsafe.o \
           $(APP)/warcdump.o ${MKTEMP}.o $(gzlib) $(PRIVATE)/wgetopt.o

arc2warc = $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
	   $(PRIVATE)/whdline.o $(PRIVATE)/wanvl.o $(PRIVATE)/wrecord.o \
	   $(PRIVATE)/wfile.o  $(PRIVATE)/wfsmhdl.o \
           $(PRIVATE)/wfsmanvl.o $(PRIVATE)/wcsafe.o $(PRIVATE)/arecord.o \
           $(PRIVATE)/afsmhdl.o $(PRIVATE)/afile.o $(TIGER)/tiger.o \
		$(PRIVATE)/wuuid.o \
	   $(APP)/arc2warc.o ${MKTEMP}.o  $(gzlib) $(PRIVATE)/wgetopt.o

warcfilter  = $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/whdline.o $(PRIVATE)/wanvl.o $(PRIVATE)/wrecord.o \
		   $(PRIVATE)/wfile.o  $(PRIVATE)/wfsmhdl.o \
           $(PRIVATE)/wfsmanvl.o $(PRIVATE)/wcsafe.o \
           $(APP)/warcfilter.o ${MKTEMP}.o $(gzlib) $(PRIVATE)/wgetopt.o

warcvalidator = $(PRIVATE)/wclass.o $(PRIVATE)/wstring.o $(PRIVATE)/wlist.o \
		   $(PRIVATE)/whdline.o $(PRIVATE)/wanvl.o $(PRIVATE)/wrecord.o \
		   $(PRIVATE)/wfile.o  $(PRIVATE)/wfsmhdl.o \
           $(PRIVATE)/wfsmanvl.o $(PRIVATE)/wcsafe.o \
           $(APP)/warcvalidator.o ${MKTEMP}.o $(gzlib) $(PRIVATE)/wgetopt.o

####################
# applications 
####################

$(APP)/warcdump:      $(warcdump);       $(CC) $(CFLAGS) -o $@ $(warcdump)
$(APP)/arc2warc:      $(arc2warc);       $(CC) $(CFLAGS) -o $@ $(arc2warc)
$(APP)/warcfilter:    $(warcfilter);     $(CC) $(CFLAGS) -o $@ $(warcfilter)
$(APP)/warcvalidator: $(warcvalidator);  $(CC) $(CFLAGS) -o $@ $(warcvalidator)

##############
# freshing
##############

tclean:		;   rm -f compress* uncompress* *.core

clean:		tclean
			rm -f $t $(obj) *.o *~ *.a *.so *.log \
	            $(PUBLIC)/*~ $(PRIVATE)/*~ $(PLUGIN)/*~ $(GZIP)/*~ \
		    $(APP)/*~ $(APP)/*.exe $(TST)/*~ $(TST)/*.exe $(DOC)/*~ \
			$(WIN32DEP)/*~ $(TIGER)/*~ semantic.cache depend \
			*.gz misc/*~ $(MISC)/DEBIAN/*~
			rm -rf $(DOC)/html warc-tools*

.PHONY: all static clean tclean doc source tgz rpm deb
