/***********************************************************************************************************************
 * DefaultEnvisionManager.cpp
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "DefaultEnvisionManager.h"
#include "EnvisionException.h"

namespace Envision {

DefaultEnvisionManager::DefaultEnvisionManager() :
	pm(NULL), exitSet(false)
{
}

DefaultEnvisionManager::~DefaultEnvisionManager()
{
	exit();
}

QList<PluginInfo> DefaultEnvisionManager::getAllLoadedPluginsInfo()
{
	if (exitSet) return QList<PluginInfo>();
	if (pm) return pm->getAllLoadedPluginsInfo();
	throw EnvisionException("The Envision Manager has no Plugin Manager set");
}

QMainWindow* DefaultEnvisionManager::getMainWindow()
{
	return mainWindow;
}



void DefaultEnvisionManager::setPluginManager(PluginManager* pm_)
{
	pm = pm_;
}

void DefaultEnvisionManager::setMainWindow(QMainWindow* mainWindow_)
{
	mainWindow = mainWindow_;
}

void DefaultEnvisionManager::exit()
{
	exitSet = true;
}

}
