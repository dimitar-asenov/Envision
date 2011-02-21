/***********************************************************************************************************************
 * VVariableDeclaration.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VVariableDeclaration.h"

#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VVariableDeclaration)

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

	if (type_ == NULL)
	{
		type_ = renderer()->render(NULL, node()->type());
		layout()->prepend(type_);
	}

	if (node()->initialValue())
	{
		if (initialValue_ == NULL || initialValue_->node() != node()->initialValue())
		{
			if (initialValue_)
			{
				layout()->remove(layout()->length()-1);
				layout()->remove(layout()->length()-1);
			}

			initialValue_ = renderer()->render(NULL, node()->initialValue());
			assignmentSymbol_ = new Symbol(NULL, &style()->assignmentSymbol() );
			layout()->append(assignmentSymbol_);
			layout()->append(initialValue_);
		}

		assignmentSymbol_->setStyle( &style()->assignmentSymbol());
	}
	else
	{
		if (initialValue_)
		{
			layout()->remove(layout()->length()-1);
			layout()->remove(layout()->length()-1);
		}
	}
}

}
