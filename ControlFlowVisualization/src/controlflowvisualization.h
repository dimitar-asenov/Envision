/***********************************************************************************************************************
 * controlflowvisualization.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CONTROLFLOWVISUALIZATION_H
#define CONTROLFLOWVISUALIZATION_H

#include "EnvisionPlugin.h"

namespace ControlFlowVisualization {

/**
 * Implements the interface between the ControlFlowVisualization plug-in and Envision.
 *
 * The Envision core will use this interface to communicate with the plug-in. The plug-in will be initialized before
 * any other operations are performed.
 *
 * The plug-in can use the supplied EnvisionManager object to find out more about the running environment.
 */
class ControlFlowVisualization : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // CONTROLFLOWVISUALIZATION_H
