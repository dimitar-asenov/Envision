/***********************************************************************************************************************
 * logger.h
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 *
 **********************************************************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include "EnvisionPlugin.h"

namespace Logger {

class LoggerPlugin : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // LOGGER_H
