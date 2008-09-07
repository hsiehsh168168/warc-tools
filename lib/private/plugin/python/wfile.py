#####################################################################
##                                                                 ##
##  wfile.py                                                      ##
##                                                                 ##
##  A Python wrapping class for the WFile C class of the Warctools ##
##                                                                 ##
## By: Amazouz Lyes. lyesjob@gmail.com                             ##
##                                                                 ##
#####################################################################

import warc
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

class WFile:

##Constructor ##

	def __init__(self, fname, max_size, mode, compressed, dname):
		self.classtype=WTypes()
		self.me = warc.bless_WFile(fname, max_size, mode, compressed, dname)
		self.type = self.classtype.WFile


## Warc Record Recovering ##

	def hasMoreRecords(self):
		return warc.WFile_hasMoreRecords(self.me)



	def nextRecord(self):
		r = WRecord()
		r.external_set(self, warc.WFile_nextRecord(self.me))
		return r



## Warc File info settings ##

	def setMaxSize (self, size):
		return warc.WFile_setMaxSize(self.me, size)


	def storeRecord (self, record):
		if (record.type == self.classtype.WRecord):
			r = record.getInternal(self)
			return warc.WFile_storeRecord(self.me, r)
		else:
			return (false)



	def seek(self, offset):
		return warc.WFile_seek(self.me, offset)


## Python WFile class particular methods

	def type(self):
		return self.classtype
 

	def getInternal(self, r):
		if (r.type == self.classtype.WRecord or r.type == self.classtype.WBloc):
			return self.me
		else:
			return 0
 
 

## Destructor ##

	def destroy(self):
		warc.destroy(self.me)



