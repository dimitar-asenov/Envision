/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * AutoCompleteVis.cpp
 *
 *  Created on: Jul 24, 2012
 *      Author: Dimitar Asenov
 */

#include "AutoCompleteVis.h"
#include "AutoComplete.h"
#include "AutoCompleteEntry.h"
#include "../vis/TextAndDescription.h"

#include "VisualizationBase/src/cursor/Cursor.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/CustomSceneEvent.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(AutoCompleteVis, "item")

AutoCompleteVis::AutoCompleteVis(const QList<AutoCompleteEntry*>& entries, const StyleType* style) :
	LayoutProvider<>(nullptr, style),
	entries_(entries),
	noProposals_(),
	watched_(),
	selectionEffect_(),
	selectionIndex_(entries_.isEmpty() ? -1 : 0)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemClipsChildrenToShape);
	setZValue(LAYER_AUTOCOMPLETE_Z);

	for (auto e : entries_)
		if (!e->visualization())
			e->setVisualization( new TextAndDescription(e->text(), e->description()));

	if (selectionIndex_ == 0) select();
}

AutoCompleteVis::~AutoCompleteVis()
{
	layout()->clear(false);
	for(auto e : entries_) SAFE_DELETE(e);
	entries_.clear();

	selectionEffect_ = nullptr; // This is deleted when it's corresponding item is deleted

	// Deleted by layout's destructor
	noProposals_ = nullptr;
}

void AutoCompleteVis::determineChildren()
{
	if (layout()->length() == 0)
	{
		if (entries_.isEmpty())
			layout()->synchronizeFirst(noProposals_, true, &style()->noProposals());
		else
			for (auto e : entries_)
				layout()->append( e->visualization());
	}

	layout()->setStyle(&style()->layout());
	if (noProposals_) noProposals_->setStyle(&style()->noProposals());
}

void AutoCompleteVis::updateGeometry(int /*availableWidth*/, int /*availableHeight*/)
{
	// Set position
	if (pos().isNull() && scene() && scene()->mainCursor() && !this->isAncestorOf(scene()->mainCursor()->owner()))
	{
		setPos(scene()->mainCursor()->region().bottomLeft() + QPoint(0, style()->distanceToCursor()));
		if (watched_ != scene()->mainCursor()->owner())
		{
			if (watched_) watched_->removeSceneEventFilter(this);
			watched_ = scene()->mainCursor()->owner();
			watched_->installSceneEventFilter(this);
		}
	}

	// Set size
	if (hasShape())
	{
		getShape()->setOffset(0, 0);

		QSize s = layout()->size().toSize();
		if (s.height() > style()->heightLimit()) s.setHeight(style()->heightLimit());
		getShape()->setInnerSize(s.width(), s.height());
		layout()->setPos(getShape()->contentLeft(), getShape()->contentTop());
	}
	else
	{
		layout()->setPos(0,0);
		QSize s = layout()->size().toSize();
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

bool AutoCompleteVis::sceneEventFilter(QGraphicsItem* /*watched*/, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		auto e = dynamic_cast<QKeyEvent*>(event);
		switch (e->key())
		{
			case Qt::Key_Down:
				{
					if (selectionIndex_ < entries_.size() - 1)
					{
						++selectionIndex_;
						select();
					}
					return true;
				}
				break;
			case Qt::Key_Up:
				{
					if (selectionIndex_ > 0)
					{
						--selectionIndex_;
						select();
					}
					return true;
				}
				break;
			case Qt::Key_Return:
			case Qt::Key_Enter:
				{
					if (selectionIndex_ < 0 && entries_.size() == 1) selectionIndex_ = 0;
					if (entries_.size() > 0) entries_.at(selectionIndex_)->execute();
					scene()->addPostEventAction(new Visualization::CustomSceneEvent( AutoComplete::hide ));
					return true;
				}
				break;
		}
	}

	return false;
}

void AutoCompleteVis::select()
{
	if (!selectionEffect_) selectionEffect_ = new QGraphicsColorizeEffect();
	entries_.at(selectionIndex_)->visualization()->setGraphicsEffect(selectionEffect_);
}

} /* namespace Interaction */
