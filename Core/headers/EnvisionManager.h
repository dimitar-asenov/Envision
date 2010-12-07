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

class EnvisionManager
{
	public:
		virtual QList<PluginInfo> getAllLoadedPluginsInfo() = 0;
		virtual QMainWindow* getMainWindow() = 0;
		virtual ~EnvisionManager() {};
};

}

#endif /* ENVISIONMANAGER_H_ */
