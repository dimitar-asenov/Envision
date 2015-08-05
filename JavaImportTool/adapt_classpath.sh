#!/bin/bash 

# Tries to adapt the .classpath file matching to your system.

# Standard plug-in path can be overriden by argument
PREFERRED_PLUGIN_PATHS=("/opt/eclipse/plugins" "/usr/share/eclipse/plugins" "/usr/lib/eclipse/plugins")

if [ ! -f .classpath ]
then
	echo ".classpath file not found!"
	exit 1
fi

PLUGIN_PATH=${PREFERRED_PLUGIN_PATHS[0]}
if [ $# == 1 ]
then
	PLUGIN_PATH=$1
else
	for eclipsePath in "${PREFERRED_PLUGIN_PATHS[@]}"
	do
		if [ -d $eclipsePath ]
		then
			PLUGIN_PATH=$eclipsePath
			break
		fi
	done
fi

if [ ! -d $PLUGIN_PATH ]
then
	echo "Eclipse plug-in path does not exist!"
	exit 1
fi

echo "Using Eclipse plug-in directory: " $PLUGIN_PATH

JDT_CORE="org.eclipse.jdt.core_"
CORE_RUNTIME="org.eclipse.core.runtime_"
CORE_JOBS="org.eclipse.core.jobs_"
CORE_CONTENTTYPE="org.eclipse.core.contenttype_"
CORE_RESOURCES="org.eclipse.core.resources_"
EQUINOX_COMMON="org.eclipse.equinox.common_"
EQUINOX_PREF="org.eclipse.equinox.preferences_"
OSGI="org.eclipse.osgi_"
TEXT="org.eclipse.text_"

JARS=($JDT_CORE $CORE_RUNTIME $CORE_JOBS $CORE_CONTENTTYPE $CORE_RESOURCES $EQUINOX_COMMON $EQUINOX_PREF $OSGI $TEXT)

for jar in "${JARS[@]}"
do
	NEW_NAME=$(basename $(find $PLUGIN_PATH -type f -name $jar"*" 2> /dev/null | head -1))
	REGEX=$jar"[^ ]*.jar"
	sed -i "s/$REGEX/$NEW_NAME/g" .classpath
done

LIB_PATH=lib
if [ -d $LIB_PATH ]
then
	COMMONS_IO="commons-io"
	NEW_COMMONS_IO=$(basename $(find $LIB_PATH -maxdepth 1 -type f -name $COMMONS_IO"*" 2>/dev/null | head -1))
	sed -i "s/${COMMONS_IO}[^ ]*.jar/$NEW_COMMONS_IO/g" .classpath
	COMMONS_LANG3="commons-lang3"
	NEW_COMMONS_LANG3=$(basename $(find $LIB_PATH -maxdepth 1 -type f -name $COMMONS_LANG3"*" 2>/dev/null | head -1))
	sed -i "s/${COMMONS_LANG3}[^ ]*.jar/$NEW_COMMONS_LANG3/g" .classpath
fi