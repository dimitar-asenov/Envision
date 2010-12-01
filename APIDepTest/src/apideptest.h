/***********************************************************************************************************************
 * apideptest.h
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 *
 * The APIDepTest plug-in tests if it can import the API provided by the Hello World plug-in and print a message. It is
 * also test the functioanlity of the core module to load all dependent plug-ins first.
 **********************************************************************************************************************/

#ifndef APIDEPTEST_H
#define APIDEPTEST_H

#include "EnvisionPlugin.h"

namespace APIDepTest {

/**
 * Implements the interface between the HelloWorld plug-in and Envision.
 *
 * The Envision core will use this interface to communicate with the plug-in. The plug-in will be initialized before
 * any other operations are performed.
 *
 * The plug-in can use the supplied EnvisionManager object to find out more about the running environment.
 */
class APIDep : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // APIDEPTEST_H
