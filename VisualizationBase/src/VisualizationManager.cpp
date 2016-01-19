/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "VisualizationManager.h"
#include "views/MainView.h"
#include "Scene.h"

namespace Visualization {

VisualizationManager::VisualizationManager() :
	envisionManager_{nullptr}, mainScene_{nullptr}, mainView_{nullptr}
{
}

VisualizationManager& VisualizationManager::instance()
{
	static VisualizationManager theInstance;
	return theInstance;
}

void VisualizationManager::init(Core::EnvisionManager *manager)
{
	if (envisionManager_ == nullptr && manager)
	{
		envisionManager_ = manager;
		mainScene_ = new Scene{};
		// It gets automatically added as a top level view.
		mainView_ = new MainView{mainScene_};
	}
}

void VisualizationManager::cleanup()
{
	views_.removeAll(mainView_);
	SAFE_DELETE(mainView_);
	SAFE_DELETE(mainScene_);
	envisionManager_ = nullptr;
}

QWidget* VisualizationManager::getMainWindow()
{
	return envisionManager_->getMainWindow();
}

void VisualizationManager::addTopLevelView(View* view)
{
	views_.append(view);

	// If this is the only view maximize it
	if (views_.size() == 1)
		envisionManager_->getMainWindow()->setCentralWidget(view);
}

}
