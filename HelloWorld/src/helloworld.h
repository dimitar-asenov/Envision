/***********************************************************************************************************************
 * helloworld.h
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 *
 * This file is an example of how to define the interface to an Envision plug-in.
 **********************************************************************************************************************/

#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include "EnvisionPlugin.h"

namespace Hello {

/**
 * Implements the interface between the HelloWorld plug-in and Envision.
 *
 * The Envision core will use this interface to communicate with the plug-in. The plug-in will be initialized before
 * any other operations are performed.
 *
 * The plug-in can use the supplied EnvisionManager object to find out more about the running environment.
 */
class HelloWorld : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // HELLOWORLD_H
