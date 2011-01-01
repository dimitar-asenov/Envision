/***********************************************************************************************************************
 * EnvisionManager.h
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ENVISIONMANAGER_H_
#define ENVISIONMANAGER_H_

#include "PluginInfo.h"

#include <QtGui/QMainWindow>
#include <QtCore/QString>

namespace Envision {

/**
 * The EnvisionManager interface provides various information about the Envision system.
 *
 * An instance of an object implementing this interface is passed to each plug-in that is loaded in the system.
 */
class EnvisionManager
{
	public:

		/**
		 * Returns a list of all the plug-ins currently loaded in the system.
		 *
		 * If this method is called during the initialization phase of Envision, only the plug-ins loaded so far will be
		 * included in the list. If a list of all plug-ins is needed, make sure to call this method after Envision has
		 * been fully initialized.
		 */
		virtual QList<PluginInfo> getAllLoadedPluginsInfo() = 0;

		/**
		 * Returns the main window of the application.
		 *
		 * This can be used by visualization plug-ins to draw custom components.
		 */
		virtual QMainWindow* getMainWindow() = 0;

		virtual ~EnvisionManager() {};
};

}

#endif /* ENVISIONMANAGER_H_ */
