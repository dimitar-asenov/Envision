/***********************************************************************************************************************
 * PluginManager.h
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PLUGINMANAGER_H_
#define PLUGINMANAGER_H_

#include <QString>
#include <QList>
#include <QPluginLoader>
#include <QDir>
#include <QMap>

#include "PluginInfo.h"
#include "EnvisionPlugin.h"

namespace Envision
{

class EnvisionManager;

class PluginManager
{
	private:
		QDir pluginsDir;
		QList<PluginInfo> pluginMetaData;
		QList<QPluginLoader*> loadedPlugins;
		QMap<QString, PluginInfo*> idToMetaDataMap;
		QMap<QString, QPluginLoader*> idToPluginLoaderMap;

	public:

		/**
		 * Creates a new plug-in manager and reads the metadata files of all plug-ins in the specified plug-in folder.
		 */
		PluginManager(QString path = "plugins/");

		/**
		 * Loads and initializes all plug-ins found in the plug-ins folder. Before a plug-in is loaded all its
		 * dependencies are loaded first. Each plug-in is given a reference to the EnvisionManager object to communicate
		 * to the core.
		 */
		void loadAllPlugins(EnvisionManager&);

		/**
		 * Returns a list containing copies of the metadata for all plug-ins which are currently loaded in the system.
		 */
		QList<PluginInfo> getAllLoadedPluginsInfo();

		/**
		 * Returns true if the specified plugin is loaded.
		 */
		bool isPluginLoaded(QString pluginId);

		/**
		 * Returns the interface of a loaded plug-in. This can be used to make calls into the plugin.
		 */
		EnvisionPlugin* getLoadedPluginInterface(QString pluginId);

	private:
		QString getLibraryFileName(const QString pluginId);
		void scanAllPluginsMetaData();
		PluginInfo readPluginMetaData(const QString fileName);
};

}

#endif /* PLUGINMANAGER_H_ */
