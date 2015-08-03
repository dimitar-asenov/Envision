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
    
    d.putNumChild(4)
    if d.isExpanded():
        with Children(d):
            d.putSubItem("data1", value["data1"])
            d.putSubItem("data2", value["data2"])
            d.putSubItem("data3", value["data3"])
            d.putSubItem("data4", value["data4"])
            
def qdump__FilePersistence__ChangeDescription(d, value):
    this = d.makeExpression(value)
    stringValue = gdb.parse_and_eval("%s.summary()" % this)
    
    d.putStringValue(stringValue)
    
    d.putNumChild(7)
    if d.isExpanded():
        with Children(d):
            d.putSubItem("nodeId_", value["nodeId_"])
            d.putSubItem("type_", value["type_"])
            d.putSubItem("updateFlags_", value["updateFlags_"])
            d.putSubItem("pointsToChildA_", value["pointsToChildA_"])
            d.putSubItem("pointsToChildB_", value["pointsToChildB_"])
            d.putSubItem("nodeA_", value["nodeA_"])
            d.putSubItem("nodeB_", value["nodeB_"])
