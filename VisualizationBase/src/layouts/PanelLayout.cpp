/***********************************************************************************************************************
 * PanelLayout.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "PanelLayout.h"

namespace Visualization {

PanelLayout::PanelLayout(Item* parent, PanelLayoutStyle* style_) :
	Item(parent), style(style_)
{
}

void PanelLayout::setStyle(PanelLayoutStyle* style_)
{
	style = style_;
	setUpdateNeeded();
}

void PanelLayout::updateState()
{
	bool horizontal = style->direction() == PanelLayoutStyle::LeftToRight ;

	// TODO this
}

}
