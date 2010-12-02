/***********************************************************************************************************************
 * filepersistence.h
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 *
 * This file is an example of how to define the interface to an Envision plug-in.
 **********************************************************************************************************************/

#ifndef FILEPERSISTENCE_H
#define FILEPERSISTENCE_H

#include "EnvisionPlugin.h"

namespace FilePersistence {

class FilePersistence : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // FILEPERSISTENCE_H
