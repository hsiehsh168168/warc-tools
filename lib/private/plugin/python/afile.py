#####################################################################
##                                                                 ##
##  Afile.py                                                      ##
##                                                                 ##
##  A Python wrapping class for the AFile C class of the Warctools ##
##                                                                 ##
## By: Amazouz Lyes. lyesjob@gmail.com                             ##
##                                                                 ##
#####################################################################

import arc
import warc
from arecord import ARecord
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



class AFile:

##Constructor ##

	def __init__(self, fname, compressed, dname):
		self.classtype=WTypes()
		self.me = arc.bless_AFile(fname, compressed, dname)
		self.type = self.classtype.AFile


## Warc Record Recovering ##

	def hasMoreRecords(self):
		if (arc.AFile_hasMoreRecords(self.me) == 0):
			return (False)
		else:
			return (True)



	def nextRecord(self):
		ar = ARecord()
		ar.external_set(self, arc.AFile_nextRecord(self.me))
		return ar



	def seek(self, offset):
		return arc.AFile_seek(self.me, offset)


## Python AFile class particular methods

	def type(self):
		return self.classtype
 

	def getInternal(self, r):
		if (r.type == self.classtype.ARecord):
			return self.me
		else:
			return 0
 
 

## Destructor ##

	def destroy(self):
		arc.destroy(self.me)



