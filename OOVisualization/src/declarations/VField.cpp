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

#include "VField.h"
#include "../OOVisualizationException.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VField, "item")

VField::VField(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

int VField::determineForm()
{
	return node()->initialValue() ? 1 : 0;
}

void VField::initializeForms()
{
	auto typeAndName = (new GridLayoutFormElement{})
		->setHorizontalSpacing(3)
		->setNoBoundaryCursors([](Item*){return true;})
		->setNoInnerCursors([](Item*){return true;})
		->put(0, 0, item(&I::type_, [](I* v){return v->node()->typeExpression();}))
		->put(1, 0, item<VText>(&I::name_, [](I* v){return v->node()->nameNode();}, [](I* v)
			{
				auto modifiers = v->node()->modifiers();
				if (modifiers->isSet(Modifier::Static))
				{
					if (modifiers->isSet(Modifier::Private)) return &v->style()->nameStaticPrivate();
					else if (modifiers->isSet(Modifier::Protected)) return &v->style()->nameStaticProtected();
					else if (modifiers->isSet(Modifier::Public)) return &v->style()->nameStaticPublic();
					else return &v->style()->nameStaticDefault();
				}
				else
				{
					if (modifiers->isSet(Modifier::Private)) return &v->style()->namePrivate();
					else if (modifiers->isSet(Modifier::Protected)) return &v->style()->nameProtected();
					else if (modifiers->isSet(Modifier::Public)) return &v->style()->namePublic();
					else return &v->style()->nameDefault();
				}

			}));

	addForm(typeAndName);

	auto alsoInitialization = typeAndName->clone();
	alsoInitialization
			->put(2, 0, item<Static>(&I::assignmentSymbol_, [](I* v){ return &v->style()->assignmentSymbol();}))
			->put(3, 0, item(&I::initialValue_, [](I* v) {return v->node()->initialValue();}));

	addForm(alsoInitialization);
}

}
