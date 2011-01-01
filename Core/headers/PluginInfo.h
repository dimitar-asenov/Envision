/***********************************************************************************************************************
 * PluginInfo.h
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PLUGININFO_H_
#define PLUGININFO_H_

#include <QtCore/QString>
#include <QtCore/QList>

namespace Envision {

/**
 * Specifies a plug-in dependency.
 */
struct PluginDependency
{
	QString id;
	QString majorVersion;
};


/**
 * Descibres a plug-in and specifies which plug-ins it depends on.
 */
struct PluginInfo
{
	QString id;
	QString fullName;
	QString version;	// FORMAT: Major.Minor.other.version.numbers.or.strings
	QList<PluginDependency> dependencies;
};

}


#endif /* PLUGININFO_H_ */
