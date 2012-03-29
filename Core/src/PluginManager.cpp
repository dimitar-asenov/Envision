/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * PluginManager.cpp
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "EnvisionPlugin.h"
#include "PluginManager.h"
#include "EnvisionException.h"

namespace Envision {

PluginManager::PluginManager(QString path) :
	pluginsDir(path)
{
	scanAllPluginsMetaData();
}

void PluginManager::loadAllPlugins(EnvisionManager& envisionManager)
{
	int lastCountLoaded = -1;

	// Holds the ids of plugins which are not yet loaded.
	QStringList plugins;
	for (QList<PluginInfo>::iterator p = pluginMetaData.begin(); p != pluginMetaData.end(); p++)
		plugins.append(p->id);

	while ( lastCountLoaded != loadedPlugins.length() && loadedPlugins.length() < pluginMetaData.length() )
	{
		lastCountLoaded = loadedPlugins.length();

		for (int i = plugins.length() - 1; i >= 0; i--)
		{
			QList<PluginDependency> depList = idToMetaDataMap.value(plugins.at(i))->dependencies;

			bool allDependenciesLoaded = true;
			for (QList<PluginDependency>::iterator dep = depList.begin(); dep != depList.end(); dep++)
			{
				// Check if this dependency is already loaded
				if ( idToPluginLoaderMap.contains(dep->id) == false )
				{
					allDependenciesLoaded = false;
					break;
				}

				// Check if the version of the non-dependent plugin matches the version the current plugin depends on
				if ( idToMetaDataMap.value(dep->id)->version.startsWith(dep->majorVersion + ".", Qt::CaseSensitive) == false )
				{
					throw EnvisionException("Plugin " + plugins.at(i) + " depends on version " + dep->majorVersion + " of " + dep->id + " but a different version is installed.");
				}
			}

			if ( allDependenciesLoaded )
			{
				QTextStream out(stdout);
				out << "Loading plug-in " << plugins.at(i) << "... ";

				QPluginLoader* plugin = new QPluginLoader(pluginsDir.absoluteFilePath(getLibraryFileName(plugins.at(i))));
				plugin->setLoadHints(QLibrary::ExportExternalSymbolsHint);

				plugin->load();

				if ( plugin->isLoaded() == false ) throw EnvisionException("Could not load plugin: " + plugin->errorString());

				EnvisionPlugin* p = qobject_cast<EnvisionPlugin*> (plugin->instance());
				p->initialize(envisionManager);

				loadedPlugins.append(plugin);
				idToPluginLoaderMap.insert(plugins.at(i), plugin);
				plugins.removeAt(i);

				out << "OK" << endl;
			}
		}
	}

	// Check if there are any plug-ins with unmet dependencies
	if (plugins.size() > 0)
	{
		QTextStream out(stdout);
		out<< "Warning: The following plug-ins have not been loaded because their dependencies are not satisfied" << endl;
		for (int i = 0; i< plugins.size(); ++i) out << "  " << plugins.at(i) << endl;
	}
}

bool PluginManager::isPluginLoaded(QString pluginId)
{
	return idToPluginLoaderMap.contains(pluginId);
}

EnvisionPlugin* PluginManager::getLoadedPluginInterface(QString pluginId)
{
	if ( isPluginLoaded(pluginId) )
	{
		return qobject_cast<EnvisionPlugin*> (idToPluginLoaderMap.value(pluginId)->instance());
	}
	else
		return nullptr;
}

QString PluginManager::getLibraryFileName(const QString pluginId)
{
#ifdef Q_WS_X11
	return "lib" + pluginId + ".so";
#endif

#ifdef Q_WS_WIN
	return pluginId + ".dll";
#endif
}

void PluginManager::scanAllPluginsMetaData()
{
	QStringList meta = pluginsDir.entryList(QStringList() << "*.plugin", QDir::Files);

	for (QStringList::iterator plugin = meta.begin(); plugin != meta.end(); plugin++)
	{
		PluginInfo info = readPluginMetaData(pluginsDir.absoluteFilePath(*plugin));

		if ( idToMetaDataMap.contains(info.id) ) throw EnvisionException("Plugin id is not unique: " + info.id);

		pluginMetaData.append(info);
		idToMetaDataMap.insert(info.id, &pluginMetaData.last());
	}
}

PluginInfo PluginManager::readPluginMetaData(const QString fileName)
{
	QDomDocument pluginDoc("EnvisionPlugin");
	QFile file(fileName);

	if ( !file.open(QIODevice::ReadOnly) ) throw EnvisionException("Can not open file for reading: " + fileName);
	if ( !pluginDoc.setContent(&file) ) throw EnvisionException("File is not a valid Envision Plugin descriptor: " + fileName);

	QDomElement root = pluginDoc.documentElement();
	if ( root.tagName() != "plugin" ) throw EnvisionException("File is not a valid Envision Plugin descriptor - Root element is invalid: " + fileName);

	PluginInfo pinfo;

	pinfo.id = root.attribute("id");
	if ( pinfo.id == QString::null ) throw EnvisionException("File is not a valid Envision Plugin descriptor - ID is invalid: " + fileName);

	pinfo.fullName = root.attribute("name");
	if ( pinfo.fullName == QString::null ) throw EnvisionException("File is not a valid Envision Plugin descriptor - Name is invalid: " + fileName);

	pinfo.version = root.attribute("version");
	if ( pinfo.version == QString::null ) throw EnvisionException("File is not a valid Envision Plugin descriptor - Version is invalid: " + fileName);

	QDomNode node = root.firstChild();
	while ( !node.isNull() )
	{
		QDomElement elem = node.toElement();
		if ( !elem.isNull() && elem.tagName() == "dependencies" )
		{
			QDomNode dep = elem.firstChild();
			while ( !dep.isNull() )
			{
				QDomElement depElem = dep.toElement();
				if ( depElem.tagName() == "dependency" )
				{
					PluginDependency pd;

					pd.id = depElem.attribute("pluginid");
					if ( pd.id == QString::null ) throw EnvisionException("Invalid plugin dependency id in file: " + fileName);

					pd.majorVersion = depElem.attribute("version");
					if ( pd.majorVersion == QString::null ) throw EnvisionException("Invalid plugin dependency version in file: " + fileName);

					pinfo.dependencies.append(pd);
				}

				dep = dep.nextSibling();
			}
		}

		node = node.nextSibling();
	}

	return pinfo;
}

QList<PluginInfo> PluginManager::getAllLoadedPluginsInfo()
{
	QList<PluginInfo> result;

	for (QList<PluginInfo>::iterator p = pluginMetaData.begin(); p != pluginMetaData.end(); p++)
	{
		if ( idToPluginLoaderMap.contains(p->id) ) result.append(*p);
	}

	return result;
}

}
