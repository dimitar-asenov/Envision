/***********************************************************************************************************************
 * PanelBorderLayout.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelBorderLayout.h"

namespace Visualization {

PanelBorderLayout::PanelBorderLayout(Item* parent, PanelBorderLayoutStyle* style_) :
		Layout(parent), style(style_), top_(new PanelLayout(this, &style->topStyle())), left_(NULL), bottom_(NULL), right_(NULL), content_(NULL)
{
}

void PanelBorderLayout::setStyle(PanelBorderLayoutStyle* style_)
{
	style = style_;
	setUpdateNeeded();
}

void PanelBorderLayout::setPanel(bool enable, PanelLayout*& panel, PanelLayoutStyle& style)
{
	if ( enable && !panel ) panel = new PanelLayout(this, &style);
	if ( !enable && panel )
	{
		delete panel;
		panel = NULL;
	}
	setUpdateNeeded();
}

void PanelBorderLayout::setContent(Item* content)
{
	if ( content_ ) delete content_;
	if ( content ) content->setParentItem(this);
	content_ = content;
	setUpdateNeeded();
}

void PanelBorderLayout::updateState()
{
	// TODO this
}

}
