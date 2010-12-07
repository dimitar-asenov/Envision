/***********************************************************************************************************************
 * DefaultEnvisionManager.h
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef DEFAULTENVISIONMANAGER_H_
#define DEFAULTENVISIONMANAGER_H_

#include "EnvisionManager.h"
#include "PluginManager.h"

namespace Envision {

class DefaultEnvisionManager: public EnvisionManager
{
	private:
		PluginManager* pm;
		QMainWindow* mainWindow;

		bool exitSet;

	public:
		DefaultEnvisionManager();
		~DefaultEnvisionManager();

		QList<PluginInfo> getAllLoadedPluginsInfo();
		QMainWindow* getMainWindow();

		void setPluginManager(PluginManager*);
		void setMainWindow(QMainWindow*);

		void exit();
};

}

#endif /* DEFAULTENVISIONMANAGER_H_ */
