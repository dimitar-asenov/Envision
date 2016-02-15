/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#pragma once

#include "PluginInfo.h"

namespace Core
{
class EnvisionManager;
class EnvisionPlugin;

class CORE_API PluginManager
{
	public:

		/**
		 * Creates a new plug-in manager and reads the metadata files of all plug-ins in the specified plug-in folder.
		 */
		PluginManager(QString path = "plugins/");

		/**
		 * Unloads all plug-ins.
		 */
		~PluginManager();

		/**
		 * Loads and initializes all plug-ins found in the plug-ins folder. Before a plug-in is loaded all its
		 * dependencies are loaded first. Each plug-in is given a reference to the EnvisionManager object to communicate
		 * to the core.
		 */
		void loadAllPlugins(EnvisionManager& envisionManager);

		/**
		 * Unloads all currently loaded plug-ins.
		 *
		 * Before a plug-in is unloaded it's unload() method will be called giving the plug-in a chance to clean up.
		 */
		void unloadAllPlugins();

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
		QDir pluginsDir;
		QList<PluginInfo> pluginMetaData;
		QList<QPluginLoader*> loadedPlugins;
		QMap<QString, PluginInfo*> idToMetaDataMap;
		QMap<QString, QPluginLoader*> idToPluginLoaderMap;
		/**
		 * lower to mixed case maps.
		 */
		QMap<QString, QString> _allFoundSharedLibraryFiles;

		QString getLibraryFileName(const QString pluginId);
		void scanAllPluginsMetaData();
		void scanSharedLibraries();
		PluginInfo readPluginMetaData(const QString fileName);
};

}
