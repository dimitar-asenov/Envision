/***********************************************************************************************************************
 * EnvisionPlugin.h
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ENVISIONPLUGIN_H_
#define ENVISIONPLUGIN_H_

#include "EnvisionManager.h"

#include <QtCore/QtPlugin>
#include <QtCore/QString>

namespace Envision {

/**
 * This EnvisionPlugin interface is the one that each plug-in must implement.
 *
 * The Envision core will look for this interface (via the QT plug-in framework) in each plug-in.
 */
class EnvisionPlugin
{
	public:

		/**
		 * Initializes the plug-in.
		 *
		 * This function is called by the core system as soon as the plug-in is loaded. All initializatoin should happen
		 * within this function.
		 *
		 * @param manager
		 * 				The EnvisionManager object that can be used to query various aspects of the Envision system.
		 *
		 * @returns
		 * 				True if the initialization was successful, false otherwise.
		 */
		virtual bool initialize(EnvisionManager& manager) = 0;

		/**
		 * Starts the indicated self-test for this plug-in.
		 *
		 * If the Envision system was started with a command line argument that indicates a particular plug-in should be
		 * tested, this method will be invoked for that plug-in. Optionally a particular test can be requested.
		 *
		 * This method will only be invoked once all plug-ins are loaded.
		 *
		 * To request that a plug-in be tested use the --test option when starting envision.
		 *
		 * E.G. to run all tests of the File persistence module:
		 * Envision --test filepersistence
		 *
		 * to only run the 'load' test:
		 * Envision --test filepersistence:load
		 *
		 * @param testid
		 * 				The string id of the test to run. If this string is empty all tests should be run.
		 */
		virtual void selfTest(QString testid) = 0;

		virtual ~EnvisionPlugin() {};
};

}

Q_DECLARE_INTERFACE(Envision::EnvisionPlugin, "EnvisionPlugin/1.0")

#endif /* ENVISIONPLUGIN_H_ */
