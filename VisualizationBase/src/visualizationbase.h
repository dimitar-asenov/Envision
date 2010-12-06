/***********************************************************************************************************************
 * visualizationbase.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 *
 **********************************************************************************************************************/

#ifndef VISUALIZATIONBASE_H
#define VISUALIZATIONBASE_H

#include "EnvisionPlugin.h"

namespace Visualization {

class VisualizationBase : public QObject, public Envision::EnvisionPlugin
{
	Q_OBJECT
	Q_INTERFACES(Envision::EnvisionPlugin)

	public:
		bool initialize(Envision::EnvisionManager&);
		void selfTest(QString testid);
};

}

#endif // VISUALIZATIONBASE_H
