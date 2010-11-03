/***********************************************************************************************************************
 * modelbase.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELBASE_H
#define MODELBASE_H

#include "EnvisionPlugin.h"

namespace Model {

class ModelBase : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // MODELBASE_H
