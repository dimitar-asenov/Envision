/***********************************************************************************************************************
 * VVariableDeclaration.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VVariableDeclaration.h"

#include "OOModel/headers/statements/VariableDeclaration.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VVariableDeclaration)

VVariableDeclaration::VVariableDeclaration(Item* parent, VariableDeclaration* node, const VVariableDeclarationStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->container()) ),
	name_(new VText(NULL, node->nameNode(), &style->name()) ),
	type_(NULL),
	assignmentSymbol_(NULL),
	initialValue_(NULL)
{
	container_->append(name_);
}

VVariableDeclaration::~VVariableDeclaration()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	name_ = NULL;
	type_ = NULL;
	assignmentSymbol_ = NULL;
	initialValue_ = NULL;
}

void VVariableDeclaration::determineChildren()
{
	VariableDeclaration* node = static_cast<VariableDeclaration*> (getNode());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->container());
	name_->setStyle( &style()->name());

	if (type_ == NULL)
	{
		type_ = renderer()->render(NULL, node->type());
		container_->prepend(type_);
	}

	if (node->initialValue())
	{
		if (initialValue_ == NULL || initialValue_->getNode() != node->initialValue())
		{
			if (initialValue_)
			{
				container_->remove(container_->length()-1);
				container_->remove(container_->length()-1);
			}

			initialValue_ = renderer()->render(NULL, node->initialValue());
			assignmentSymbol_ = new Symbol(NULL, &style()->assignmentSymbol() );
			container_->append(assignmentSymbol_);
			container_->append(initialValue_);
		}

		assignmentSymbol_->setStyle( &style()->assignmentSymbol());
	}
	else
	{
		if (initialValue_)
		{
			container_->remove(container_->length()-1);
			container_->remove(container_->length()-1);
		}
	}
}

void VVariableDeclaration::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VVariableDeclaration::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}

}
