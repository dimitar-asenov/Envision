/***********************************************************************************************************************
 * VExtendable.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VExtendable.h"

#include "items/Text.h"
#include "shapes/Box.h"

#include <QtCore/QString>
#include <QtCore/QPair>
#include <QtCore/QList>

namespace Visualization {

VExtendable::VExtendable(Item* parent, Model::ExtendableNode* node, const VExtendableStyle* style) :
	ModelItem(parent, node, style), layout(this, &style->borderStyle()), header(NULL, &style->headerStyle()),
			attributes(NULL, &style->attributesStyle())
{
	layout.setTop(true);
	layout.top()->setMiddle(&header);
	layout.setContent(&attributes);
	header.append(new Text(&header, node->getTypeName()));
}

void VExtendable::determineChildren()
{
	Model::ExtendableNode* node = static_cast<Model::ExtendableNode*> (getNode());

	// Set the name if any
	if ( node->hasAttribute("name") )
	{
		Model::Node* name = node->get("name");
		if ( header.length() == 1 ) header.prepend(renderer()->render(&header, name));
		if ( header.length() == 2 && header.at<ModelItem> (0)->getNode() != name )
		{
			header.remove(0);
			header.prepend(renderer()->render(&header, name));
		}
	}
	else
	{
		if ( header.length() > 1 ) header.remove(0);
	}

	// Set the attributes
	// TODO this can be done smarter
	QList<QPair<QString, Model::Node*> > attr = node->getAllAttributes();

	bool changed = false;
	for (int i = 0; i < attr.size(); ++i)
	{
		if ( !changed )
		{
			changed = attributes.length() <= i;
			if ( !changed ) changed = attributes.at<SequentialLayout> (i)->at<ModelItem> (1)->getNode() != attr[i].second;

			if ( changed ) for (int k = i; k < attributes.length(); ++k)
				attributes.remove(attributes.length() - 1);
		}

		if ( changed )
		{
			SequentialLayout* s = new SequentialLayout(&attributes);
			s->append(new Text(s, attr[i].first));
			s->append(renderer()->render(s, attr[i].second));
			attributes.append(s);
		}
	}

}

void VExtendable::updateState()
{
	if ( getShape() )
	{
		getShape()->setOffset(layout.getXOffsetForExternalShape(), layout.getYOffsetForExternalShape());
		getShape()->setOutterSize(layout.getOutterWidthForExternalShape(), layout.getOutterHeightForExternalShape());
	}
	size .setHeight(layout.height());
	size.setWidth(layout.width());
}

}
