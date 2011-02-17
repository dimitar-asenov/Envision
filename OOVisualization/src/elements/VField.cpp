/***********************************************************************************************************************
 * VField.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VField.h"
#include "OOVisualizationException.h"
#include "elements/VVisibility.h"

#include "OOModel/headers/top_level/Field.h"

#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VField)

VField::VField(Item* parent, Field* node, const VFieldStyle* style) :
	ModelItem(parent, node, style),
	container_( new SequentialLayout(this, &style->container()) ),
	name_(new VText(NULL, node->nameNode(), &style->nameDefault()) ),
	type_(NULL)
{
	container_->append(name_);
}

VField::~VField()
{
	SAFE_DELETE_ITEM(container_);

	// These were automatically deleted by container's destructor
	name_ = NULL;
	type_ = NULL;
}

void VField::determineChildren()
{
	Field* node = static_cast<Field*> (getNode());

	if (type_ == NULL)
	{
		type_ = renderer()->render(NULL,node->type() );
		container_->append(type_);
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	container_->setStyle( &style()->container() );

	if (node->stat() == Static::INSTANCE_VARIABLE)
	{
		if (node->visibility() == Visibility::DEFAULT) name_->setStyle( &style()->nameDefault());
		else if (node->visibility() == Visibility::PRIVATE) name_->setStyle( &style()->namePrivate());
		else if (node->visibility() == Visibility::PROTECTED) name_->setStyle( &style()->nameProtected());
		else if (node->visibility() == Visibility::PUBLIC) name_->setStyle( &style()->namePublic());
		else throw OOVisualizationException("Unknown visibility type in VField::determineChildren");
	}
	else if (node->stat() == Static::CLASS_VARIABLE)
	{
		if (node->visibility() == Visibility::DEFAULT) name_->setStyle( &style()->nameStaticDefault());
		else if (node->visibility() == Visibility::PRIVATE) name_->setStyle( &style()->nameStaticPrivate());
		else if (node->visibility() == Visibility::PROTECTED) name_->setStyle( &style()->nameStaticProtected());
		else if (node->visibility() == Visibility::PUBLIC) name_->setStyle( &style()->nameStaticPublic());
		else throw OOVisualizationException("Unknown visibility type in VField::determineChildren");
	}
	else throw OOVisualizationException("Unknown static type in VField::determineChildren");

}

void VField::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(container_, availableWidth, availableHeight);
}

bool VField::focusChild(FocusTarget location)
{
	return container_->focusChild(location);
}
}
