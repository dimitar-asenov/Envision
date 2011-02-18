/***********************************************************************************************************************
 * VAssignmentStatement.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VAssignmentStatement.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VAssignmentStatement)

VAssignmentStatement::VAssignmentStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, AssignmentStatement>(parent, node, style),
	assignmentSymbol_( new Symbol(NULL, &style->op( node->op() ).inSymbol()) ),
	left_(NULL),
	right_(NULL)
{
	layout()->append(assignmentSymbol_);
}

VAssignmentStatement::~VAssignmentStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	assignmentSymbol_ = NULL;
	left_ = NULL;
	right_ = NULL;
}

void VAssignmentStatement::determineChildren()
{
	if (!left_)
	{
		left_ = renderer()->render(NULL, node()->left());
		layout()->prepend(left_);
		right_ = renderer()->render(NULL, node()->right());
		layout()->append(right_);
	}


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->op(node()->op()).container());
	assignmentSymbol_->setStyle( &style()->op(node()->op()).inSymbol());
}

}
