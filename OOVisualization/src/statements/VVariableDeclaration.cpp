/***********************************************************************************************************************
 * VVariableDeclaration.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VVariableDeclaration.h"

#include "VisualizationBase/headers/items/Static.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VVariableDeclaration, "item")

VVariableDeclaration::VVariableDeclaration(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, VariableDeclaration>(parent, node, style),
	name_(new VText(NULL, node->nameNode(), &style->name()) ),
	type_(NULL),
	assignmentSymbol_(NULL),
	initialValue_(NULL)
{
	layout()->append(name_);
}

VVariableDeclaration::~VVariableDeclaration()
{
	// These were automatically deleted by LayoutProvider's destructor
	name_ = NULL;
	type_ = NULL;
	assignmentSymbol_ = NULL;
	initialValue_ = NULL;
}

void VVariableDeclaration::determineChildren()
{
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	name_->setStyle( &style()->name());

	layout()->synchronizeFirst(type_, node()->type());
	layout()->synchronizeMid(name_, node()->nameNode(), &style()->name(), 1);
	layout()->synchronizeMid(assignmentSymbol_, node()->initialValue() != NULL, &style()->assignmentSymbol(), 2);
	layout()->synchronizeLast(initialValue_, node()->initialValue());
}

}
