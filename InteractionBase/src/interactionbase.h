/***********************************************************************************************************************
 * interactionbase.h
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef INTERACTIONBASE_H
#define INTERACTIONBASE_H

#include "EnvisionPlugin.h"

#include "Logger/headers/Log.h"

namespace Interaction {

/**
 * Implements the interface between the InteractionBase plug-in and Envision.
 *
 * The Envision core will use this interface to communicate with the plug-in. The plug-in will be initialized before
 * any other operations are performed.
 *
 * The plug-in can use the supplied EnvisionManager object to find out more about the running environment.
 */
class InteractionBase : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);

		static Logger::Log* log();

	private:
		static Logger::Log* logger;
};

}

#endif // INTERACTIONBASE_H
