This folder contains code examples used for the evaluation of Envision's version control system.
Each .tar file contains a git repository and some files. The name of the tar file indicates which is the corresponding envision project in Envision/FilePersistence/test/persisted/version-control/.
More information about the examples can be found in Balz Guenat's bachlor thesis.

ClassMove: One branch moves a class to a new package. The other makes a few minor changes. Git detects a conflict, Envision merges successfully.

LoopFix: Both branches fix a bug, git merges successfully, causing a bug. Envision detects conflict.

MethodInsert: Both branches create a new method. git detects a conflict. Envision merges successfully.