/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "items/VExtendable.h"
#include "items/Text.h"
#include "../renderer/ModelRenderer.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VExtendable, "item")

VExtendable::VExtendable(Item* parent, NodeType* node, const StyleType* style) : BaseItemType(parent, node, style),
	header( new SequentialLayout(this, &style->smallHeaderStyle())),
	layout(),
	attributes(),
	expanded_(style->expanded())
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
		header = nullptr;
		attributes = nullptr;
	}
	else SAFE_DELETE_ITEM(header);
}

void VExtendable::determineChildren()
{
	// Set the name if any
	if ( node()->hasAttribute("name") )
	{
		Model::Node* name = node()->get("name");
		if ( header->length() == 1 ) header->prepend(renderer()->render(header, name));
		if ( header->length() == 2 && header->at<Item>(0)->node() != name )
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

			layout->setTop(true);
			layout->top()->setMiddle(header);

			attributes = new SequentialLayout(layout, &style()->attributesStyle());
			layout->setContent(attributes);
		}
		else
		{
			removeShape();
			// This is the header. We do not want this to be removed by layout's destructor
			layout->top()->setMiddle(nullptr, false);
			header->setParentItem(this);

			SAFE_DELETE_ITEM(layout);

			attributes = nullptr; // This was automatically deleted by layout's destructor
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
		// NOTE: This procedure assumes that the order of the named attributes will never change. Attributes might be
		// deleted or added but their ordering should be kept constant.
		// TODO: document this somewhere more visible.
		QList<QPair<QString, Model::Node*> > attr = node()->getAllAttributes();

		// Remove elements from attributes which are outdated
		for(int i = attributes->length() - 1; i>=0; --i)
		{
			bool found = false;
			for (int k = 0; k<attr.size(); ++k)
			{
				if (attr[k].second == attributes->at<SequentialLayout>(i)->at<Item>(1)->node())
				{
					found = true;
					break;
				}
			}

			if (!found) attributes->remove(i);
		}

		int attributeIndex = 0;
		for (int i = 0; i < attr.size(); ++i)
		{
			if ( attr[i].first == "name" ) continue;

			if (attributes->length() <= attributeIndex || attributes->at<SequentialLayout>(attributeIndex)->at<Item>(1)->node() != attr[i].second)
			{
				SequentialLayout* s = new SequentialLayout(attributes);
				s->append(new Text(s, attr[i].first));
				s->append(renderer()->render(s, attr[i].second));
				attributes->insert(s, attributeIndex);
			}

			++attributeIndex;
		}
	}

}

void VExtendable::updateGeometry(int availableWidth, int availableHeight)
{
	if ( expanded_ ) Item::updateGeometry(layout, availableWidth, availableHeight);
	else Item::updateGeometry(header, availableWidth, availableHeight);
}

void VExtendable::setExpanded(bool expanded)
{
	if ( expanded != expanded_ )
	{
		expanded_ = expanded;
		setUpdateNeeded(FullUpdate);
	}
}

inline bool VExtendable::expandedSwtiched() const
{
	return (layout && attributes) != expanded_;
}

}
