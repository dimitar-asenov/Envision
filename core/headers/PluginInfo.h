/***********************************************************************************************************************
 * PluginInfo.h
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PLUGININFO_H_
#define PLUGININFO_H_

#include <QString>
#include <QList>

namespace Envision {

struct PluginDependency
{
	QString id;
	QString majorVersion;
};

struct PluginInfo
{
	QString id;
	QString fullName;
	QString version;
	QList<PluginDependency> dependencies;
};

}


#endif /* PLUGININFO_H_ */
