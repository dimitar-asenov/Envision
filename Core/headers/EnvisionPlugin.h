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

class EnvisionPlugin
{
	public:
		virtual bool initialize(EnvisionManager&) = 0;
		virtual void selfTest(QString testid) = 0;
		virtual ~EnvisionPlugin() {};
};

}

Q_DECLARE_INTERFACE(Envision::EnvisionPlugin, "EnvisionPlugin/1.0")

#endif /* ENVISIONPLUGIN_H_ */
