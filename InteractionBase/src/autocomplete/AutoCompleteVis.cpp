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

#include "AutoCompleteVis.h"
#include "AutoComplete.h"
#include "AutoCompleteEntry.h"
#include "../vis/TextAndDescription.h"
#include "../vis/CommandPrompt.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/cursor/Cursor.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/CustomSceneEvent.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(AutoCompleteVis, "item")

AutoCompleteVis::AutoCompleteVis(const QList<AutoCompleteEntry*>& entries, const StyleType* style) :
Super(nullptr, style), newEntries_(entries)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemClipsChildrenToShape);
	setZValue(LAYER_AUTOCOMPLETE_Z);
	setItemCategory(Visualization::Scene::MenuItemCategory);
}

AutoCompleteVis::~AutoCompleteVis()
{
	layout()->clear(false);
	SAFE_DELETE_ITEM(noProposals_);
	for(auto e : entries_) SAFE_DELETE(e);
	for(auto e : newEntries_) SAFE_DELETE(e);
	entries_.clear();
	newEntries_.clear();

	selectionEffect_ = nullptr; // This is deleted when its corresponding item is deleted

	// Deleted by layout's destructor
	noProposals_ = nullptr;
}

void AutoCompleteVis::setExplicitSelection(bool explicitSelection)
{
	explicitSelection_ = explicitSelection;
}

void AutoCompleteVis::setEntries(const QList<AutoCompleteEntry*>& entries)
{
	newEntries_ = entries;
	newEntriesSet_ = true;
}

void AutoCompleteVis::updateEntries()
{
	newEntriesSet_ = false;
	layout()->clear(false);
	SAFE_DELETE_ITEM(noProposals_);
	for(auto e : entries_) SAFE_DELETE(e);
	entries_ = newEntries_;
	newEntries_.clear();

	for (auto e : entries_)
			if (!e->visualization())
				e->setVisualization( new TextAndDescription(e->text(), e->description()));

	if (entries_.isEmpty()) layout()->synchronizeFirst(noProposals_, true, &style()->noProposals());
	else for (auto e : entries_) layout()->append(e->visualization().data());

	// Install event handler
	if (scene() && scene()->mainCursor() && !this->isAncestorOf(scene()->mainCursor()->owner()))
		scene()->mainCursor()->owner()->installSceneEventFilter(this);
}

void AutoCompleteVis::determineChildren()
{
	if (newEntriesSet_)
	{
		updateEntries();
		resetSelection();
	}

	layout()->setStyle(&style()->layout());
	if (noProposals_) noProposals_->setStyle(&style()->noProposals());
}

void AutoCompleteVis::updateGeometry(int /*availableWidth*/, int /*availableHeight*/)
{
	// Set position
	if (scene() && scene()->mainCursor() && !this->isAncestorOf(scene()->mainCursor()->owner()))
	{
		auto itemToUseForPosition = scene()->mainCursor()->owner();

		// Check if this is a command prompt
		auto root = itemToUseForPosition;
		while (root->parent()) root = root->parent();
		if (dynamic_cast<CommandPrompt*>(root))
			itemToUseForPosition = root;

		// The cursor does not belong to a command prompt
		setPos(itemToUseForPosition->scenePos().toPoint() +
			QPoint(0, itemToUseForPosition->heightInScene() + style()->distanceToCursor()));
	}

	// Set size
	if (hasShape())
	{
		getShape()->setOffset(0, 0);

		QSize s = layout()->sizeInParent().toSize();
		if (s.height() > style()->heightLimit()) s.setHeight(style()->heightLimit());
		getShape()->setInnerSize(s.width(), s.height());
		layout()->setPos(getShape()->contentLeft(), getShape()->contentTop());
	}
	else
	{
		layout()->setPos(0,0);
		QSize s = layout()->sizeInParent().toSize();
		if (s.height() > style()->heightLimit()) s.setHeight(style()->heightLimit());
		setSize(s);
	}
}

Visualization::Item::UpdateType AutoCompleteVis::needsUpdate()
{
	auto upd = Visualization::LayoutProvider<>::needsUpdate();
	if ( upd == NoUpdate)
		return StandardUpdate;
	else return upd;
}

bool AutoCompleteVis::sceneEventFilter(QGraphicsItem* watched, QEvent* event)
{
	Visualization::Item* cursorOwner = nullptr;

	if (scene()->mainCursor() && !this->isAncestorOf(scene()->mainCursor()->owner()))
		cursorOwner = scene()->mainCursor()->owner();

	if (event->type() == QEvent::FocusOut)
		AutoComplete::hide();

	if (cursorOwner != watched)
	{
		watched->removeSceneEventFilter(this);
		return false;
	}

	if (event->type() == QEvent::KeyPress && !entries_.isEmpty())
	{
		auto e = dynamic_cast<QKeyEvent*>(event);
		switch (e->key())
		{
			case Qt::Key_Down:
				{
					selectDown();
					return true;
				}
				break;
			case Qt::Key_Up:
				{
					selectUp();
					return true;
				}
				break;
			case Qt::Key_Return:
			case Qt::Key_Enter:
				{
					auto executed = false;

					if (selectionIndex_>=0 && selectionIndex_ < entries_.size())
					{
						executed = true;
						entries_.at(selectionIndex_)->execute();
					}
					scene()->addPostEventAction(new Visualization::CustomSceneEvent( AutoComplete::hide ));
					return executed || !explicitSelection_;
				}
				break;
		}
	}

	return false;
}

void AutoCompleteVis::resetSelection()
{
	selectionEffect_ = nullptr;
	selectionIndex_ = -1;
	if (!explicitSelection_ && entries_.size() > 0)
	{
		selectionIndex_ = 0;
	}
	updateSelection();
}

void AutoCompleteVis::selectUp()
{
	auto newIndex = selectionIndex_ - 1;
	if (newIndex <= -1)
	{
		if (explicitSelection_) newIndex = -1;
		else if (entries_.size() > 0) newIndex = 0;
	}

	if (newIndex != selectionIndex_)
	{
		selectionIndex_ = newIndex;
		updateSelection();
	}
}

void AutoCompleteVis::selectDown()
{
	auto newIndex = selectionIndex_ + 1;
	if (newIndex < entries_.size())
	{
		selectionIndex_ = newIndex;
		updateSelection();
	}
}

void AutoCompleteVis::updateSelection()
{
	if (selectionIndex_ >= 0 && selectionIndex_ < entries_.size())
	{
		// There is a selected item.
		if (!selectionEffect_)
			selectionEffect_ = new QGraphicsColorizeEffect();	// Note we must renew this every time since it will be
																				// automatically deleted by the item that owns it.

		entries_.at(selectionIndex_)->visualization()->setGraphicsEffect(selectionEffect_);
	}
	else
	{
		// There is no selected item.
		if (selectionEffect_)
		{
			Visualization::Item* itemWithEffect = nullptr;
			for (auto i : entries_)
				if (auto eff = i->visualization()->graphicsEffect())
				{
					itemWithEffect = i->visualization().data();
					Q_ASSERT(eff == selectionEffect_);
					itemWithEffect->setGraphicsEffect(nullptr);
					selectionEffect_ = nullptr;
					break;
				}
			Q_ASSERT(itemWithEffect);
		}
	}
}

} /* namespace Interaction */
