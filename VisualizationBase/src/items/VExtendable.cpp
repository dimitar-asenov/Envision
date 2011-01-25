/***********************************************************************************************************************
 * VExtendable.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VExtendable.h"

#include "items/Text.h"

#include <QtCore/QString>
#include <QtCore/QPair>
#include <QtCore/QList>

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VExtendable)

VExtendable::VExtendable(Item* parent, Model::ExtendableNode* node, const VExtendableStyle* style) :
	ModelItem(parent, node, style), header( new SequentialLayout(this, &style->headerStyle())), layout(NULL), attributes(NULL)
{
	header->append(new Text(header, node->typeName()));
}

VExtendable::~VExtendable()
{
	if ( style()->expanded() )
	{
		SAFE_DELETE_ITEM(layout);

		// These were automatically deleted by layout's destructor
		header = NULL ;
		attributes = NULL;
	}
	else SAFE_DELETE_ITEM(header);
}

void VExtendable::determineChildren()
{
	Model::ExtendableNode* node = static_cast<Model::ExtendableNode*> (getNode());

	// Set the name if any
	if ( node->hasAttribute("name") )
	{
		Model::Node* name = node->get("name");
		if ( header->length() == 1 ) header->prepend(renderer()->render(header, name));
		if ( header->length() == 2 && header->at<ModelItem> (0)->getNode() != name )
		{
			header->remove(0);
			header->prepend(renderer()->render(header, name));
		}
	}
	else
	{
		if ( header->length() > 1 ) header->remove(0);
	}

	// Clean up when switching styles
	if ( expandedSwtiched() )
	{
		if ( style()->expanded() )
		{
			layout = new PanelBorderLayout(this, &style()->borderStyle());
			attributes = new SequentialLayout(NULL, &style()->attributesStyle());

			layout->setTop(true);
			layout->top()->setMiddle(header);
			layout->setContent(attributes);
		}
		else
		{
			// This is the header. We do not want this to be removed by layout's destructor
			layout->top()->setMiddle(NULL, false);
			header->setParentItem(this);

			SAFE_DELETE_ITEM(layout);

			attributes = NULL; // This was automatically deleted by layout's destructor
		}
	}

	// TODO: find a better way and place to determine the style of children
	header->setStyle(&style()->headerStyle());

	if ( style()->expanded() )
	{
		layout->setStyle(&style()->borderStyle());
		attributes->setStyle(&style()->attributesStyle());

		// Set the attributes
		// TODO this can be done smarter
		QList<QPair<QString, Model::Node*> > attr = node->getAllAttributes();

		bool changed = false;
		int nameFound = 0; // This will become 1 if the name attribute was already encountered
		for (int i = 0; i < attr.size(); ++i)
		{
			if ( attr[i].first == "name" )
			{
				nameFound = 1;
				continue;
			}

			if ( !changed )
			{
				changed = attributes->length() + nameFound <= i;
				if ( !changed ) changed = attributes->at<SequentialLayout> (i - nameFound)->at<ModelItem> (1)->getNode() != attr[i].second;

				if ( changed ) while ( attributes->length() + nameFound > i )
					attributes->remove(attributes->length() - 1);
			}

			if ( changed )
			{
				SequentialLayout* s = new SequentialLayout(attributes);
				s->append(new Text(s, attr[i].first));
				s->append(renderer()->render(s, attr[i].second));
				attributes->append(s);
			}
		}
	}

}

void VExtendable::updateGeometry(int, int)
{
	if ( style()->expanded() )
	{
		if ( getShape() )
		{
			getShape()->setOffset(layout->getXOffsetForExternalShape(), layout->getYOffsetForExternalShape());
			getShape()->setOutterSize(layout->getOutterWidthForExternalShape(), layout->getOutterHeightForExternalShape());
		}
		setSize( layout->size() );
	}
	else
	{
		if ( getShape() )
		{
			getShape()->setOffset(0, 0);
			getShape()->setInnerSize(header->width(), header->height());
			header->setPos(getShape()->contentLeft(), getShape()->contentTop());
		}
		else
		{
			setSize(header->size());
			header->setPos(0,0);
		}
	}
}

void VExtendable::setExpanded(bool expanded)
{
	if ( expanded != style()->expanded() )
	{
		if ( expanded ) setStyle(Styles::item<VExtendable>("expanded"));
		else setStyle(Styles::item<VExtendable>("default"));
	}
}

inline bool VExtendable::expandedSwtiched() const
{
	return (layout && attributes) != style()->expanded();
}

}
