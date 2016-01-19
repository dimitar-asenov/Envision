/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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
#include "VViewSwitcherEntry.h"

#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/ViewItemManager.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(VViewSwitcherEntry, "item")

VViewSwitcherEntry::VViewSwitcherEntry(Visualization::Item* parent, QString viewName, const StyleType* style) :
		Super{parent, style}
{
	nameField_ = new Visualization::Text{this, viewName};
	nameField_->setEditable(true);
	oldName_ = viewName;
}

void VViewSwitcherEntry::initializeForms()
{
	addForm(item(&I::nameField_));
}

void VViewSwitcherEntry::determineChildren()
{
	Super::determineChildren();
	auto view = scene()->viewItems()->viewItem(oldName_);
	if (view && Visualization::ViewItem::isValidName(nameField_->text())) view->setName(nameField_->text());
	setStyle(view == scene()->currentViewItem() ? itemStyles().get("currentView")
						: (view ? itemStyles().get("existingView") : itemStyles().get("default")));
	oldName_ = nameField_->text();
}
}
