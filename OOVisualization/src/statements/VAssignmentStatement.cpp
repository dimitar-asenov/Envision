/***********************************************************************************************************************
 * VAssignmentStatement.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VAssignmentStatement.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VAssignmentStatement, "item")

VAssignmentStatement::VAssignmentStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, AssignmentStatement>(parent, node, style),
	assignmentSymbol_( new Static(NULL, &style->op( node->op() ).inSymbol()) ),
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
	layout()->synchronizeFirst(left_, node()->left());
	layout()->synchronizeMid(assignmentSymbol_, true, &style()->op( node()->op() ).inSymbol(), 1);
	layout()->synchronizeLast(right_, node()->right());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->op(node()->op()).layout());
	assignmentSymbol_->setStyle( &style()->op(node()->op()).inSymbol());
}

}
