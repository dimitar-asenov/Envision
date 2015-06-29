# This file is a GDB / QT Creator debug helper that pretty prints QUuid values in the format:
# 
# {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
#
# This was developed as a response to
# http://stackoverflow.com/questions/31124259/making-a-gdb-debugging-helper-for-the-quuid-class
#
# You can enable this debug helper script in Qt Creator by going to
# Tools > Options... > Debugger > GDB > Extra Debugging Helpers > Browse and selecting this file.

from dumper import *
import gdb
    
def qdump__QUuid(d, value):
    this = d.makeExpression(value)
    stringValue = gdb.parse_and_eval("%s.toString()" % this)
    
    d.putStringValue(stringValue)
    d.putNumChild(0)