/***********************************************************************************************************************
 * PLUGINNAME_LOWERCASE.h
 *
 *  Created on: DATE
 *      Author: AUTHOR
 **********************************************************************************************************************/

#ifndef PLUGINNAME_UPPERCASE_H
#define PLUGINNAME_UPPERCASE_H

#include "EnvisionPlugin.h"

namespace NAMESPACE {

/**
 * Implements the interface between the PLUGINNAME plug-in and Envision.
 *
 * The Envision core will use this interface to communicate with the plug-in. The plug-in will be initialized before
 * any other operations are performed.
 *
 * The plug-in can use the supplied EnvisionManager object to find out more about the running environment.
 */
class PLUGINNAME : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // PLUGINNAME_UPPERCASE_H
