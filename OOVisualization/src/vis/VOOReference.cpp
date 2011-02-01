/***********************************************************************************************************************
 * VOOReference.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/VOOReference.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VOOReference)

VOOReference::VOOReference(Item* parent, OOModel::OOReference *ref, const Visualization::TextStyle *style) :
	Visualization::TextRenderer<Visualization::ModelItem>(parent, ref, style)
{
}

void VOOReference::updateGeometry(int availableWidth, int availableHeight)
{
	OOModel::OOReference* ref = static_cast<OOModel::OOReference*> (getNode());
	setText( ref->path());
	Visualization::TextRenderer<Visualization::ModelItem>::updateGeometry(availableWidth, availableHeight);
}

}
