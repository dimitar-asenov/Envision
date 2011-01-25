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
	ModelItem(parent, node, style), header( new SequentialLayout(this, &style->smallHeaderStyle())), layout(NULL), attributes(NULL), expanded_(style->expanded())
{
	header->append(new Text(header, node->typeName()));
	if (!expanded_) removeShape();
}

VExtendable::~VExtendable()
{
	if ( expanded_ )
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
		if ( expanded_ )
		{
			useShape();
			layout = new PanelBorderLayout(this, &style()->borderStyle());
			attributes = new SequentialLayout(NULL, &style()->attributesStyle());

			layout->setTop(true);
			layout->top()->setMiddle(header);
			layout->setContent(attributes);
		}
		else
		{
			removeShape();
			// This is the header. We do not want this to be removed by layout's destructor
			layout->top()->setMiddle(NULL, false);
			header->setParentItem(this);

			SAFE_DELETE_ITEM(layout);

			attributes = NULL; // This was automatically deleted by layout's destructor
		}
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	if (expanded_)
	{
		header->setStyle(&style()->headerStyle());
		layout->setStyle(&style()->borderStyle());
		attributes->setStyle(&style()->attributesStyle());
	}
	else header->setStyle(&style()->smallHeaderStyle());

	if ( expanded_ )
	{
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
	if ( expanded_ )
	{
		if ( hasShape() )
		{
			getShape()->setOffset(layout->getXOffsetForExternalShape(), layout->getYOffsetForExternalShape());
			getShape()->setOutterSize(layout->getOutterWidthForExternalShape(), layout->getOutterHeightForExternalShape());
		}
		setSize( layout->size() );
	}
	else
	{
		if ( hasShape() )
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
	if ( expanded != expanded_ )
	{
		expanded_ = expanded;
		setUpdateNeeded();
	}
}

inline bool VExtendable::expandedSwtiched() const
{
	return (layout && attributes) != expanded_;
}

}
