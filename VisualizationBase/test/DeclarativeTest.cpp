/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "DeclarativeTest.h"

#include "../src/items/Symbol.h"
#include "../src/items/TestBox.h"
#include "../src/items/VComposite.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "../src/declarative/DeclarativeItemDef.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(DeclarativeTest, "item")

DeclarativeTest::DeclarativeTest(Item* parent, TestNodes::BinaryNode* node, Model::Node* first, Model::Node* second,
		Model::Node* third, Model::Node* fourth) :
		Super(parent, itemStyles().get()), testNode_{node}, firstNode_{first},
		secondNode_{second}, thirdNode_{third}, fourthNode_{fourth}
{
	setPurpose(0);
}

void DeclarativeTest::initializeForms()
{
	FormElement* testItemElement = item<Symbol>(&I::testItem_, [](I*){return &itemStyles().get()->symbol();});
	FormElement* testNodeItemGeneralElement = item(&I::testNodeItemGeneral_, [](I* v){return v->testNode_;});
	FormElement* testNodeItemElement = item<VComposite>(&I::testNodeItem_, [](I* v){return v->testNode_;},
																		[](I*){return VComposite::itemStyles().get();});
	FormElement* firstElement = item(&I::firstItem_, [](I* v){return v->firstNode_;});
	FormElement* secondElement = item(&I::secondItem_, [](I* v){return v->secondNode_;});
	FormElement* thirdElement = item(&I::thirdItem_, [](I* v){return v->thirdNode_;});
	FormElement* fourthElement = item(&I::fourthItem_, [](I* v){return v->fourthNode_;});

	// Test 0: VisualizationItemWrapperElement
	addForm(testItemElement);

	// Test 1: NodeItemWrapperElement
	addForm(testNodeItemGeneralElement);

	// Test 2: NodeWithVisualizationItemWrapperElement
	addForm(testNodeItemElement);

	// Test 3: GridLayoutElement
	addForm((new GridLayoutFormElement{})
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setHorizontalAlignment(LayoutStyle::Alignment::Right)
				->put(0, 0, firstElement)
				->put(0, 1, testNodeItemElement)
				->put(1, 1, (new GridLayoutFormElement{})
									->put(0, 0, secondElement)
									->put(1, 1, thirdElement))
				->put(2, 0, fourthElement));

	// Test 4: GridLayoutElement with merged cells
	addForm((new GridLayoutFormElement{})
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setHorizontalAlignment(LayoutStyle::Alignment::Center)
				->setColumnStretchFactor(1, 1)
				->setRowStretchFactors(1)
				->put(0, 0, (new GridLayoutFormElement{})
									->setHorizontalSpacing(10)
									->put(0, 0, firstElement)
									->put(1, 0, secondElement)
									->put(2, 0, thirdElement))
				->setCellSpanning(2, 1)
				->put(2, 0, testNodeItemElement)
				->setCellSpanning(1, 2)
				->put(0, 1, fourthElement)
				);

	// Test 5: Size dependencies inside the grid & alignment
	addForm((new GridLayoutFormElement{})
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->put(0, 0, (new GridLayoutFormElement{})
									->setHorizontalSpacing(300)
									->put(0, 0, firstElement)
									->put(1, 0, secondElement)))
				->put(0, 1, (new GridLayoutFormElement{})
									->setColumnStretchFactor(1, 1)
									->put(0, 0, testNodeItemElement)
									->put(2, 0, thirdElement)
									->setCellVerticalAlignment(LayoutStyle::Alignment:: Center))
				->put(1, 0, fourthElement)
				->setCellSpanning(1, 2);

	// Test 6: Anchor layout
	addForm((new AnchorLayoutFormElement{})
				->put(TheBottomOf, firstElement, 20, FromTopOf, testNodeItemElement)
				->put(TheLeftOf, firstElement, AtLeftOf, testNodeItemElement)
				->put(TheVCenterOf, secondElement, AtCenterOf, firstElement)
				->put(TheLeftOf, secondElement, 20, FromRightOf, firstElement)
				->put(TheHCenterOf, thirdElement, AtCenterOf, secondElement)
				->put(TheTopOf, thirdElement, 20, FromBottomOf, testNodeItemElement)
				->put(TheTopOf, fourthElement, 0.33, thirdElement)
				->put(TheRightOf, fourthElement, 0.1, testNodeItemElement)
				);

	// Test 7: Anchor Layout with stretching Elements, in order specification
	FormElement* verticalGrid1 = (new GridLayoutFormElement)
										->setRowStretchFactors(1)
										->put(0, 0, firstElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Top)
										->put(0, 1, secondElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Center)
										->put(0, 2, thirdElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Bottom);
	FormElement* verticalGrid2 = (new GridLayoutFormElement)
										->setRowStretchFactors(1)
										->put(0, 0, fourthElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Top)
										->put(0, 1, testNodeItemElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Bottom);
	addForm((new AnchorLayoutFormElement{})
				->put(TheTopOf, verticalGrid1, AtTopOf, verticalGrid2)
				->put(TheBottomOf, verticalGrid1, AtBottomOf, verticalGrid2)
				->put(TheLeftOf, verticalGrid1, 20, FromRightOf, verticalGrid2));

	// Test 8: Anchor Layout out of order specification with more elements, circular dependencies + shape element test
	auto shapeElement = new ShapeFormElement{};
	addForm((new AnchorLayoutFormElement{})
				// arrange them on top of each other
				->put(TheTopOf, firstElement, 20, FromBottomOf, secondElement)
				->put(TheTopOf, secondElement, 20, FromBottomOf, thirdElement)
				// right edges elements 1 and 2
				->put(TheRightOf, firstElement, AtRightOf, secondElement)
				// left edges elements 3 and 2
				->put(TheLeftOf, thirdElement, AtLeftOf, secondElement)
				// left edges elements 1 and 3
				->put(TheLeftOf, firstElement, AtLeftOf, thirdElement)
				// right edges elements 1 and 3
				->put(TheRightOf, firstElement, AtRightOf, thirdElement)
				// place shape element
				->put(TheLeftOf, shapeElement, 10, FromLeftOf, secondElement)
				->put(TheRightOf, shapeElement, 10, FromRightOf, secondElement)
				->put(TheTopOf, shapeElement, 30, FromTopOf, secondElement)
				->put(TheBottomOf, shapeElement, 10, FromBottomOf, secondElement));

	// Test 9: Test sequential layout
	FormElement* horizontalSequential = (new SequentialLayoutFormElement{})
			->setSpaceBetweenElements(5)
			->setListOfItems([](Item*){return QList<Item*>{new TestBox{"a", false, false}, new TestBox{"b", false, false},
				new TestBox{"cde", false, false}};});
	FormElement* verticalSequential = (new SequentialLayoutFormElement{})
			->setSpaceBetweenElements(5)
			->setListOfNodes([](Item*){return QList<Model::Node*>{new TestBoxNode{"dfsdfs", true, false},
																					new TestBoxNode{"sfsdfsdf", false, false}};})
			->setVertical();

	addForm((new AnchorLayoutFormElement{})
			->put(TheLeftOf, horizontalSequential, 20, FromRightOf, verticalSequential));
}

int DeclarativeTest::determineForm()
{
	return 8;
}

}
