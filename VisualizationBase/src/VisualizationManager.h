/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

/***********************************************************************************************************************
 * VisualizationManager.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VISUALIZATIONMANAGER_H_
#define VISUALIZATIONMANAGER_H_

#include "visualizationbase_api.h"

#include "Core/src/EnvisionManager.h"

namespace Visualization {

class View;
class MainView;
class Scene;

class VISUALIZATIONBASE_API VisualizationManager
{
	public:
		static VisualizationManager& instance();
		VisualizationManager(const VisualizationManager& other) = delete;
		VisualizationManager&  operator = (const VisualizationManager& other) = delete;

		QWidget* getMainWindow();

		void addTopLevelView(View* view);
		void init(Core::EnvisionManager *manager);
		void cleanup();

		Scene* mainScene();
		MainView* mainView();

	private:
		VisualizationManager();

		Core::EnvisionManager *envisionManager_;
		Scene* mainScene_;
		MainView* mainView_;
		QList<View*> views_;
};

inline Scene* VisualizationManager::mainScene() { return mainScene_; }
inline MainView* VisualizationManager::mainView() { return mainView_; }
}

#endif /* VISUALIZATIONMANAGER_H_ */
