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

#include "top_level/VClass.h"
#include "OOVisualizationException.h"

#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/layouts/PositionLayout.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"

#include "VisualizationBase/src/declarative/AnchorLayoutFormElement.h"
#include "VisualizationBase/src/declarative/GridLayoutFormElement.h"
#include "VisualizationBase/src/declarative/SequentialLayoutFormElement.h"
#include "VisualizationBase/src/declarative/ShapeFormElement.h"
#include "VisualizationBase/src/items/NodeWrapper.h"

#include "ModelBase/src/nodes/Node.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VClass, "item")

VClass::VClass(Item* parent, NodeType* node, const StyleType* style) : Super(parent, node, style)
{
	setDefaultMoveCursorProxy(name_);
	body_ = new PositionLayout(this, &style->body());
}

void VClass::determineChildren()
{
	// manually update the body item
	if (body_->needsUpdate() == FullUpdate) body_->clear(true);
	QList<Model::Node*> bodyItems = node()->classes()->nodes().toList();
	bodyItems << node()->methods()->nodes().toList();
	body_->synchronizeWithNodes( bodyItems, renderer());;

	// call determineChildren of super class
	BaseItemType::determineChildren();

	// make field background behave as a special background element
	if (fieldBackground_) fieldBackground_->setStretchable(true);
}

void VClass::initializeForms()
{
	auto headerElement = (new GridLayoutFormElement())
				->setHorizontalSpacing(3)->setColumnStretchFactor(3, 1)
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->put(0, 0, item<Static, I>(&I::icon_, [](I* v){return &v->style()->icon();}))
				->put(1, 0, item<VText, I>(&I::name_, [](I* v){return v->node()->nameNode();}, [](I* v)
						{
							// return the correct name style, depending on the classes visibility
							if (v->node()->visibility() == Visibility::DEFAULT) return &v->style()->nameDefault();
							else if (v->node()->visibility() == Visibility::PUBLIC) return &v->style()->namePublic();
							else if (v->node()->visibility() == Visibility::PRIVATE) return &v->style()->namePrivate();
							else if (v->node()->visibility() == Visibility::PROTECTED) return &v->style()->nameProtected();
							else throw OOVisualizationException("Unknown visibility in VClass::initializeForms");
						}))
				->put(2, 0, item<VList, I>(&I::typeArguments_, [](I* v){return v->node()->typeArguments();},
																				[](I* v){return &v->style()->typeArguments();}))
				->put(3, 0, item<VList, I>(&I::baseClasses_, [](I* v){return v->node()->baseClasses();},
																			[](I* v){return &v->style()->baseClasses();}));

	auto contentElement = (new GridLayoutFormElement())
				->setVerticalSpacing(3)->setColumnStretchFactor(0, 1)
				->put(0, 0, item<VStatementItemList, I>(&I::annotations_, [](I* v)
											{return v->node()->annotations()->size() > 0 ? v->node()->annotations() : nullptr;},
								[](I* v){return &v->style()->annotations();}))
				->put(0, 1, item<VList, I>(&I::enumerators_, [](I* v)
											{return v->node()->enumerators()->size() > 0 ? v->node()->enumerators() : nullptr;},
								[](I* v){return &v->style()->enumerators();}))
				->put(0, 2, item<PositionLayout, I>(&I::body_, [](I* v){return &v->style()->body();}));

	auto fieldContainerElement = (new GridLayoutFormElement())
				->setVerticalSpacing(3)
				->put(0, 0, (new SequentialLayoutFormElement())->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClass*>(i))->publicFields_;}))
				->put(0, 1, (new SequentialLayoutFormElement())->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClass*>(i))->privateFields_;}))
				->put(0, 2, (new SequentialLayoutFormElement())->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClass*>(i))->protectedFields_;}))
				->put(0, 3, (new SequentialLayoutFormElement())->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClass*>(i))->defaultFields_;}));

	auto shapeElement = new ShapeFormElement();
	auto backgroundElement = item<NodeWrapper, I>(&I::fieldBackground_, [](I*){return nullptr;},
																	[](I* v){return &v->style()->fieldContainer();})
										->setCreateIfNoNode(true);

	// Form 0: with field nodes
	addForm((new AnchorLayoutFormElement())
				// place the top left corner of the field container element
				->put(TheLeftOf, fieldContainerElement, 10, FromLeftOf, headerElement)
				->put(TheTopOf, fieldContainerElement, 5, FromBottomOf, headerElement)
				// place the top left corner of the content element
				->put(TheLeftOf, contentElement, 10, FromRightOf, fieldContainerElement)
				->put(TheTopOf, contentElement, AtBottomOf, headerElement)
				// align content and header on their right side
				->put(TheRightOf, contentElement, AtRightOf, headerElement)
				// put the shape element at the right place
				->put(TheTopOf, shapeElement, AtCenterOf, headerElement)
				->put(TheLeftOf, shapeElement, AtLeftOf, headerElement)
				->put(TheBottomOf, shapeElement, 10, FromBottomOf, contentElement)
				->put(TheRightOf, shapeElement, 10, FromRightOf, headerElement)
				// put the background element around the field container element
				->put(TheLeftOf, backgroundElement, 3, FromLeftOf, fieldContainerElement)
				->put(TheRightOf, backgroundElement, 3, FromRightOf, fieldContainerElement)
				->put(TheTopOf, backgroundElement, 3, FromTopOf, fieldContainerElement)
				->put(TheBottomOf, backgroundElement, 3, FromBottomOf, fieldContainerElement));

	// Form 1: without field nodes
	addForm((new AnchorLayoutFormElement())
				// place the top left corner of the content element
				->put(TheLeftOf, headerElement, 10, FromLeftOf, contentElement)
				->put(TheTopOf, contentElement, AtBottomOf, headerElement)
				// align content and header on their right side
				->put(TheRightOf, contentElement, AtRightOf, headerElement)
				// put the shape element at the right place
				->put(TheTopOf, shapeElement, AtCenterOf, headerElement)
				->put(TheLeftOf, shapeElement, AtLeftOf, headerElement)
				->put(TheBottomOf, shapeElement, 10, FromBottomOf, contentElement)
				->put(TheRightOf, shapeElement, 10, FromRightOf, headerElement));
}

int VClass::determineForm()
{
	// Update Field Nodes
	publicFields_.clear();
	privateFields_.clear();
	protectedFields_.clear();
	defaultFields_.clear();

	for (int i = 0; i< node()->fields()->size(); ++i)
	{
		if (node()->fields()->at(i)->visibility() == Visibility::PUBLIC)
			publicFields_.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::PRIVATE)
			privateFields_.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::PROTECTED)
			protectedFields_.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::DEFAULT)
			defaultFields_.append(node()->fields()->at(i));
		else throw OOVisualizationException("Unknown visibility value when updating VClass instance.");
	}

	if (node()->fields()->size() > 0) return 0;
	else return 1;
}

}
