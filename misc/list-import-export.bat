@echo off
if not exist ..\DebugBuild\plugins (
	echo The DebugBuild\plugins directory does not exist.
	exit /B
)

if not exist ..\DebugBuild\imports mkdir ..\DebugBuild\imports
if not exist ..\DebugBuild\exports mkdir ..\DebugBuild\exports

if exist ..\DebugBuild\imports\%~1 (
	echo Folder ..\DebugBuild\imports\%~1 already exists. Aborting.
	exit /B
)
if exist ..\DebugBuild\exports\%~1 (
	echo Folder ..\DebugBuild\exports\%~1 already exists. Aborting.
	exit /B
)

mkdir ..\DebugBuild\imports\%~1
mkdir ..\DebugBuild\exports\%~1

for %%f in (..\DebugBuild\plugins\*.dll) do (
	echo Processing %%f
	dumpbin /imports %%f > ..\DebugBuild\imports\%~1\imports_%%~nf.txt
	dumpbin /exports %%f > ..\DebugBuild\exports\%~1\exports_%%~nf.txt
)

echo All Done.