#!/bin/bash

WORKSPACE_DIR=".."
DATE_FORMAT="%b %d, %Y"


# Print usage information
if [ $# != 3 ] 
then
	echo
	echo "  Usage: gen.sh PluginName PluginFullName Namespace"
	echo
	echo "    PluginName is the name of the plug-in in CamelCase (First letter is capital)."
	echo "    This is also the name of the main class for the plug-in and the name of the project."
	echo "    E.G. : HelloWorld"
	echo
	echo "    PluginFullName is a one line plug-in name/description. This is used in the plug-in"
	echo "    description meta file (.plugin)"
	echo "    E.G. : Hello World Sample Plugin"
	echo
	echo "    Namespace is the main namespace for this plug-in. This is usually the same as PluginName"
	echo "    but it doesn't have to be."
	echo "    E.G. : Hello"
	exit
fi

# Start generating
echo "Generating plugin $1 ..."

# Set Variables
PLUGIN_NAME=$1
PLUGIN_NAME_UPPER=`echo $PLUGIN_NAME | awk '{print toupper($0)}'`
PLUGIN_NAME_LOWER=`echo $PLUGIN_NAME | awk '{print tolower($0)}'`
PLUGIN_DESCRIPTION=$2
PLUGIN_NAMESPACE=$3
DATE=`date +"$DATE_FORMAT"`

PLUGIN_DIR=$WORKSPACE_DIR/$PLUGIN_NAME
FILE_LIST="$PLUGIN_DIR/*.* $PLUGIN_DIR/src/*.* $PLUGIN_DIR/test/*.*"

if [ -d $PLUGIN_DIR ]
then
	echo "ERROR: A plug-in with the name '$PLUGIN_NAME' already exists in the workspace '$WORKSPACE_DIR'"
	exit 1
fi

echo "Creating project folder from template..."
cp -R Plugin $PLUGIN_DIR
rm -rf `find $PLUGIN_DIR/ -name '.svn'`


echo "Adjusting source code..."
sed -i -e "s/PLUGINNAME_UPPERCASE/$PLUGIN_NAME_UPPER/g" $FILE_LIST
sed -i -e "s/PLUGINNAME_LOWERCASE/$PLUGIN_NAME_LOWER/g" $FILE_LIST
sed -i -e "s/PLUGINNAME/$PLUGIN_NAME/g" $FILE_LIST
sed -i -e "s/NAMESPACE/$PLUGIN_NAMESPACE/g" $FILE_LIST
sed -i -e "s/PLUGIN_FULL_NAME/$PLUGIN_DESCRIPTION/g" $FILE_LIST
sed -i -e "s/DATE/$DATE/g" $FILE_LIST

echo "Renaming files..."
mv $PLUGIN_DIR/src/plugin_api.h $PLUGIN_DIR/src/${PLUGIN_NAME_LOWER}_api.h
mv $PLUGIN_DIR/src/PluginClass.h $PLUGIN_DIR/src/${PLUGIN_NAME}Plugin.h
mv $PLUGIN_DIR/src/PluginClass.cpp $PLUGIN_DIR/src/${PLUGIN_NAME}Plugin.cpp
mv $PLUGIN_DIR/src/PluginException.h $PLUGIN_DIR/src/${PLUGIN_NAME}Exception.h
mv $PLUGIN_DIR/src/PluginException.cpp $PLUGIN_DIR/src/${PLUGIN_NAME}Exception.cpp
mv $PLUGIN_DIR/pluginmeta.plugin $PLUGIN_DIR/${PLUGIN_NAME_LOWER}.plugin
echo "DONE."
echo
echo "Note: Remember to configure the project dependencies:"
echo "  - Include additional dependencies on other plug-ins in the .plugin file"
echo "  - Include the corresponding library dependencies in the CMakeLists.txt file"
echo "  - Configure the precomliped header 'src/precompiled.h' to include the precompiled headers of libraries it uses"
echo "  - Add project to the top level CMakeLists.txt file"

