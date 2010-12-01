/***********************************************************************************************************************
 * modelbase.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELBASE_H
#define MODELBASE_H

#include "EnvisionPlugin.h"
#include "Logger/headers/Log.h"

namespace Model {

class ModelBase : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	private:
		static Logger::Log* logger;

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);

		static Logger::Log* log();
};

}

#endif // MODELBASE_H
