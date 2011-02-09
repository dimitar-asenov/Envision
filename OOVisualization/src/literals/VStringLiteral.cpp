/***********************************************************************************************************************
 * VStringLiteral.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "literals/VStringLiteral.h"

#include "ModelBase/headers/Model.h"

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStringLiteral)

VStringLiteral::VStringLiteral(Item* parent, OOModel::StringLiteral *literal, const Visualization::TextStyle *style) :
	Visualization::TextRenderer<Visualization::ModelItem>(parent, literal, style)
{
}

void VStringLiteral::updateGeometry(int availableWidth, int availableHeight)
{
	OOModel::StringLiteral* literal = static_cast<OOModel::StringLiteral*> (getNode());
	setText(literal->value());
	Visualization::TextRenderer<Visualization::ModelItem>::updateGeometry(availableWidth, availableHeight);
}

void VStringLiteral::setText(const QString& newText)
{
	OOModel::StringLiteral* literal = static_cast<OOModel::StringLiteral*> (getNode());
	literal->model()->beginModification(literal, "Set string literal value");
	literal->setValue(newText);
	literal->model()->endModification();
	Visualization::TextRenderer<ModelItem>::setText(newText);
}

}
