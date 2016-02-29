/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "VComposite.h"
#include "Text.h"
#include "EmptyItem.h"
#include "../renderer/ModelRenderer.h"

#include "../declarative/DeclarativeItem.hpp"

namespace Visualization {

DEFINE_ITEM_COMMON(VComposite, "item")

VComposite::VComposite(Item* parent, NodeType* node, const StyleType* style)
	: Super{parent, node, style}, expanded_(style->expanded())
{
	typeName_ = new Text{this, node->typeName()};
}

void VComposite::initializeForms()
{
	auto headerContent = (new GridLayoutFormElement{})
		->setHorizontalSpacing(3)
		->setColumnStretchFactor(3, 1)
		->setVerticalAlignment(LayoutStyle::Alignment::Center)
		->setNoBoundaryCursors([](Item*){return true;})
		->setNoInnerCursors([](Item*){return true;})
		->put(0, 0, item(&I::name_, [](I* v){return v->nameNode();}))
		->put(1, 0, item<Text>(&I::typeName_, [](I* v){return &v->style()->typeName();}));

	auto headerBackground = item<EmptyItem>(&I::headerBackground_, [](I* v){ return &v->style()->headerBackground();});

	auto completeHeader = grid({{(new AnchorLayoutFormElement{})
			->put(TheLeftOf, headerBackground, 2, FromLeftOf, headerContent)
			->put(TheTopOf, headerBackground, 2, FromTopOf, headerContent)
			->put(TheBottomOf, headerBackground, 2, FromBottomOf, headerContent)
			->put(TheRightOf, headerBackground, 2, FromRightOf, headerContent)}});

	addForm(completeHeader);

	auto shapeElement = new ShapeFormElement{};
	auto attr = grid({{(new SequentialLayoutFormElement{})->setVertical()
			->setListOfItems([](Item* i){return (static_cast<VComposite*>(i))->attributes();})}})
		->setColumnStretchFactor(1, 1);

	auto completeHeader2 = completeHeader->clone();
	completeHeader2->setColumnStretchFactor(1, 1);

	addForm((new AnchorLayoutFormElement{})
		// place the top left corner of the content element
		->put(TheLeftOf, completeHeader2, 10, FromLeftOf, attr)
		->put(TheTopOf, attr, AtBottomOf, completeHeader2)
		// align content and header on their right side
		->put(TheRightOf, attr, AtRightOf, completeHeader2)
		// put the shape element at the right place
		->put(TheTopOf, shapeElement, AtCenterOf, completeHeader2)
		->put(TheLeftOf, shapeElement, AtLeftOf, completeHeader2)
		->put(TheBottomOf, shapeElement, 10, FromBottomOf, attr)
		->put(TheRightOf, shapeElement, 10, FromRightOf, completeHeader2));
}

int VComposite::determineForm()
{
	int formId = expanded() ? 1 : 0;

	if (formId == 0) attributes_.clear();
	return formId;
}

Model::Node* VComposite::nameNode()
{
	if ( node()->hasAttribute("name") )
		return node()->get("name");
	else return nullptr;
}

QList<Item*> VComposite::attributes()
{
	// Set the attributes
	// NOTE: This procedure assumes that the order of the named attributes will never change. Attributes might be
	// deleted or added but their ordering should be kept constant.
	auto attr = node()->getAllAttributes();

	// Remove elements from attributes which are outdated
	for (int i = attributes_.size() - 1; i>=0; --i)
	{
		bool found = false;
		for (int k = 0; k<attr.size(); ++k)
			if (attr[k].second == static_cast<SequentialLayout*>(attributes_[i])->at<Item>(1)->node())
			{
				found = true;
				break;
			}

		if (!found) attributes_.removeAt(i);
	}

	int attributeIndex = 0;
	for (int i = 0; i < attr.size(); ++i)
	{
		if ( attr[i].first == "name" ) continue;

		if (	attributes_.size() <= attributeIndex
				|| static_cast<SequentialLayout*>(attributes_[attributeIndex])->at<Item>(1)->node() != attr[i].second)
		{
			auto s = new SequentialLayout{nullptr};
			s->append(new Text{s, attr[i].first});
			s->append(renderer()->render(s, attr[i].second));
			attributes_.insert(attributeIndex, s);
		}

		++attributeIndex;
	}

	return attributes_;
}

void VComposite::setExpanded(bool expanded)
{
	if ( expanded != expanded_ )
	{
		expanded_ = expanded;
		setUpdateNeeded(StandardUpdate);
	}
}

}
