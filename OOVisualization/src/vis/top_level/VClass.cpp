/***********************************************************************************************************************
 * VClass.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VClass.h"
#include "OOVisualizationException.h"
#include "icons/ClassIcon.h"

#include "OOModel/headers/Class.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayout.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VClass)

VClass::VClass(Item* parent, Class* node, const VClassStyle* style) :
	ModelItem(parent, node, style),
	layout_( new PanelBorderLayout(this, &style->border()) ),
	header_( new SequentialLayout(NULL, &style->header()) ),
	icon_( new ClassIcon(NULL, &style->icon()) ),
	name_(new VText(NULL, node->nameNode(), &style->nameDefault()) ),
	content_( new PositionLayout(NULL, &style->content()) ),
	fieldContainer_(new SequentialLayout(NULL, &style->fieldContainer()) ),
	publicFieldArea_(new SequentialLayout(NULL, &style->publicFieldArea()) ),
	privateFieldArea_(new SequentialLayout(NULL, &style->privateFieldArea()) ),
	protectedFieldArea_(new SequentialLayout(NULL, &style->protectedFieldArea()) ),
	defaultFieldArea_(new SequentialLayout(NULL, &style->defaultFieldArea()) )
{
	layout_->setTop(true);
	layout_->top()->setFirst(header_);
	header_->append(icon_);
	header_->append(name_);

	layout_->setLeft(true);
	layout_->left()->setFirst(fieldContainer_);
	fieldContainer_->append(publicFieldArea_);
	fieldContainer_->append(protectedFieldArea_);
	fieldContainer_->append(defaultFieldArea_);
	fieldContainer_->append(privateFieldArea_);

	layout_->setContent(content_);
}

VClass::~VClass()
{
	SAFE_DELETE_ITEM(layout_);

	// These were automatically deleted by layout's destructor
	header_ = NULL;
	icon_ = NULL;
	name_ = NULL;
	content_ = NULL;
	fieldContainer_ = NULL;
	publicFieldArea_ = NULL;
	privateFieldArea_ = NULL;
	protectedFieldArea_ = NULL;
	defaultFieldArea_ = NULL;
}

void VClass::determineChildren()
{
	Class* node = static_cast<Class*> (getNode());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout_->setStyle( &style()->border() );
	icon_->setStyle(&style()->icon());
	header_->setStyle( &style()->header() );
	content_->setStyle( &style()->content() );
	fieldContainer_->setStyle( &style()->fieldContainer() );
	publicFieldArea_->setStyle( &style()->publicFieldArea() );
	privateFieldArea_->setStyle( &style()->privateFieldArea() );
	protectedFieldArea_->setStyle( &style()->protectedFieldArea() );
	defaultFieldArea_->setStyle( &style()->defaultFieldArea() );

	if (node->visibility() == Visibility::DEFAULT) name_->setStyle( &style()->nameDefault() );
	else if (node->visibility() == Visibility::PUBLIC) name_->setStyle( &style()->namePublic() );
	else if (node->visibility() == Visibility::PRIVATE) name_->setStyle( &style()->namePrivate() );
	else if (node->visibility() == Visibility::PROTECTED) name_->setStyle( &style()->nameProtected() );
	else throw OOVisualizationException("Unknown visibility in VClass::determineChildren");

	//////////////////////////////////////////////////////////////////////////////// Methods
	// Remove methods that are outdated
	for(int i = content_->length() - 1; i>=0; --i)
	{
		bool found = false;

		for (int k = 0; k<node->methods()->size(); ++k)
			if (node->methods()->at(k) == content_->at<ModelItem>(i)->getNode())
			{
				found = true;
				break;
			}

		if (!found) content_->remove(i);
	}

	// Add new methods
	for (int k = 0; k<node->methods()->size(); ++k)
		if ( !content_->hasNode( node->methods()->at(k) ) )
			content_->insert(renderer()->render(NULL, node->methods()->at(k)));

	//////////////////////////////////////////////////////////////////////////////// Fields
	QList<Model::Node*> publicFields;
	QList<Model::Node*> privateFields;
	QList<Model::Node*> protectedFields;
	QList<Model::Node*> defaultFields;
	for (int i = 0; i< node->fields()->size(); ++i)
	{
		if (node->fields()->at(i)->visibility() == Visibility::PUBLIC) publicFields.append(node->fields()->at(i));
		else if (node->fields()->at(i)->visibility() == Visibility::PRIVATE) privateFields.append(node->fields()->at(i));
		else if (node->fields()->at(i)->visibility() == Visibility::PROTECTED) protectedFields.append(node->fields()->at(i));
		else if (node->fields()->at(i)->visibility() == Visibility::DEFAULT) defaultFields.append(node->fields()->at(i));
		else throw OOVisualizationException("Unknown visibility value when updating VClass instance.");
	}

	publicFieldArea_->synchronizeWithNodes(publicFields, renderer());
	privateFieldArea_->synchronizeWithNodes(privateFields, renderer());
	protectedFieldArea_->synchronizeWithNodes(protectedFields, renderer());
	defaultFieldArea_->synchronizeWithNodes(defaultFields, renderer());
}

void VClass::updateGeometry(int, int)
{
	if ( hasShape() )
	{
		getShape()->setOffset(layout_->getXOffsetForExternalShape(), layout_->getYOffsetForExternalShape());
		getShape()->setOutterSize(layout_->getOutterWidthForExternalShape(), layout_->getOutterHeightForExternalShape());
	}
	setSize( layout_->size() );
}

bool VClass::focusChild(FocusTarget location)
{
	return layout_->focusChild(location);
}

}
