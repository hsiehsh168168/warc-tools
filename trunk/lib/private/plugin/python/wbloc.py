#####################################################################
##                                                                 ##
##  wbloc.py                                                       ##
##                                                                 ##
##  A Python wrapping class for the WBloc C class of the warc ##
##                                                                 ##
##  By: Amazouz Lyes. lyesjob@gmail.com                            ##
##                                                                 ##
#####################################################################

import warc
from wfile import WFile
from wrecord import WRecord
import sys

sys.path.insert (0, ".")
from wtypes import WTypes

#class WTypes:
	#"A class to define Python WARC classes types"
	#def __init__(self):
		#self.WNone   = 0
		#self.WFile   = 1
		#self.WRecord = 2
		#self.WBloc   = 3
		#self.AFile   = 4
		#self.ARecord = 5

class WBloc:

##Constructor ##
	def __init__(self, wfile, wrec, alloc):
		self.classtype = WTypes()
		if (wfile.type != self.classtype.WFile or wrec.type != self.classtype.WRecord or alloc == 0):
			return 0
		self.type = self.classtype.WBloc
		self.me = warc.bless_WBloc(wfile.getInternal(self), wrec.getInternal(self), alloc)


## Bloc chunks recovering ##

	def getNext(self):
		return warc.WBloc_next(self.me)


	def getHttpCode(self):
		return warc.WBloc_getHttpCode(self.me)




## Ruby WBloc class particular methods

	def type(self):
		return self.type
 

## Destructor ##

	def destroy(self):
		warc.destroy(self.me)


##  end  ##