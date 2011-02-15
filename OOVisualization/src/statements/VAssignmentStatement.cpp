/***********************************************************************************************************************
 * VAssignmentStatement.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VAssignmentStatement.h"

#include "OOModel/headers/statements/AssignmentStatement.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VAssignmentStatement)

VAssignmentStatement::VAssignmentStatement(Item* parent, AssignmentStatement* node, const OperatorSequenceStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->op(node->op()).container()) ),
	assignmentSymbol_( new Symbol(NULL, &style->op( node->op() ).inSymbol()) ),
	left_(NULL),
	right_(NULL)
{
	container_->append(assignmentSymbol_);
}

VAssignmentStatement::~VAssignmentStatement()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	assignmentSymbol_ = NULL;
	left_ = NULL;
	right_ = NULL;
}

void VAssignmentStatement::determineChildren()
{
	AssignmentStatement* node = static_cast<AssignmentStatement*> (getNode());

	if (!left_)
	{
		left_ = renderer()->render(NULL, node->left());
		container_->prepend(left_);
		right_ = renderer()->render(NULL, node->right());
		container_->append(right_);
	}


	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->op(node->op()).container());
	assignmentSymbol_->setStyle( &style()->op(node->op()).inSymbol());
}

void VAssignmentStatement::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VAssignmentStatement::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
