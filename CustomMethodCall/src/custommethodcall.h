/***********************************************************************************************************************
 * custommethodcall.h
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CUSTOMMETHODCALL_H
#define CUSTOMMETHODCALL_H

#include "EnvisionPlugin.h"

namespace CustomMethodCall {

/**
 * Implements the interface between the CustomMethodCall plug-in and Envision.
 *
 * The Envision core will use this interface to communicate with the plug-in. The plug-in will be initialized before
 * any other operations are performed.
 *
 * The plug-in can use the supplied EnvisionManager object to find out more about the running environment.
 */
class CustomMethodCall : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // CUSTOMMETHODCALL_H
