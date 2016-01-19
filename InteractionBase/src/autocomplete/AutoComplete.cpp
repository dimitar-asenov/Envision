/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "AutoComplete.h"
#include "AutoCompleteVis.h"
#include "VisualizationBase/src/CustomSceneEvent.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"

namespace Interaction {

AutoCompleteVis* AutoComplete::vis_ = nullptr;
bool AutoComplete::hideRequested_ = false;


void AutoComplete::show(const QList<AutoCompleteEntry*> entries, bool explicitSelection )
{
	hideRequested_ = false;
	QApplication::postEvent(Visualization::VisualizationManager::instance().mainScene(),
		new Visualization::CustomSceneEvent( [=](){ showNow(entries, explicitSelection);	} ) );
}

void AutoComplete::hide()
{
	hideRequested_ = true;
	QApplication::postEvent(Visualization::VisualizationManager::instance().mainScene(),
			new Visualization::CustomSceneEvent( [&](){ if (hideRequested_) hideNow();	} ) );
}

void AutoComplete::showNow(const QList<AutoCompleteEntry*>& entries, bool explicitSelection)
{
	if (!hideRequested_)
	{
		if (vis_ && vis_->scene() == Visualization::VisualizationManager::instance().mainScene())
		{
			vis_->setExplicitSelection(explicitSelection);
			vis_->setEntries(entries);
		}
		else
		{
			if (vis_) hideNow();
			vis_ = new AutoCompleteVis{entries};
			vis_->setExplicitSelection(explicitSelection);
			Visualization::VisualizationManager::instance().mainScene()->addTopLevelItem(vis_);
		}
	}
}

void AutoComplete::hideNow()
{
	hideRequested_ = false;
	if (vis_) Visualization::VisualizationManager::instance().mainScene()->removeTopLevelItem(vis_);
	SAFE_DELETE_ITEM(vis_);
}

bool AutoComplete::isVisible()
{
	return vis_ && vis_->scene();
}

AutoCompleteVis* AutoComplete::visualization()
{
	return vis_;
}

}
