/***********************************************************************************************************************
 * VisualizationManager.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VISUALIZATIONMANAGER_H_
#define VISUALIZATIONMANAGER_H_

#include "EnvisionManager.h"

namespace Visualization {

class View;

class VisualizationManager
{
	private:
		static VisualizationManager theInstance;

		Envision::EnvisionManager *envisionManager;
		QVector<View*> views;

		VisualizationManager();
		VisualizationManager(const VisualizationManager& other);
		VisualizationManager&  operator = (const VisualizationManager& other);

	public:
		QWidget* getMainWindow();

		void addTopLevelView(View* view);

		static VisualizationManager& instance();
		static void init(Envision::EnvisionManager *manager);
};

}

#endif /* VISUALIZATIONMANAGER_H_ */
