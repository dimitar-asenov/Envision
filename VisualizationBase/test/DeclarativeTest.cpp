/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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
#include "../src/items/VExtendable.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "../src/declarative/GridLayoutElement.h"
#include "../src/declarative/AnchorLayoutElement.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(DeclarativeTest, "item")

DeclarativeTest::DeclarativeTest(Item* parent, TestNodes::BinaryNode* node) :
		DeclarativeItem<DeclarativeTest>(parent), testNode_{node}
{
	setPurpose(0);
}

void DeclarativeTest::initializeForms()
{
	Element* testItemElement = item<Symbol, I>(&I::testItem_, [](I*){return itemStyles().get();});
	Element* testItem2Element = item<Symbol, I>(&I::testItem2_, [](I*){return itemStyles().get();});
	Element* testItem3Element = item<Symbol, I>(&I::testItem3_, [](I*){return itemStyles().get();});
	Element* testItem4Element = item<Symbol, I>(&I::testItem4_, [](I*){return itemStyles().get();});
	Element* testNodeItemGeneralElement = item<I>(&I::testNodeItemGeneral_, [](I* v){return v->testNode_;});
	Element* testNodeItemElement = item<VExtendable,I>(&I::testNodeItem_, [](I* v){return v->testNode_;},
																		[](I*){return VExtendable::itemStyles().get();});

	// Test 0: VisualizationItemWrapperElement
	addForm(testItemElement);

	// Test 1: NodeItemWrapperElement
	addForm(testNodeItemGeneralElement);

	// Test 2: NodeWithVisualizationItemWrapperElement
	addForm(testNodeItemElement);

	// Test 3: GridLayoutElement
	addForm((new GridLayoutElement())
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setHorizontalAlignment(LayoutStyle::Alignment::Right)
				->put(0, 0, testItemElement)
				->put(0, 1, testNodeItemElement)
				->put(1, 1, (new GridLayoutElement())
									->put(0, 0, testItem2Element)
									->put(1, 1, testItem3Element))
				->put(2, 0, testItem4Element));

	// Test 4: GridLayoutElement with merged cells
	addForm((new GridLayoutElement())
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setHorizontalAlignment(LayoutStyle::Alignment::Center)
				->setColumnStretchFactor(1, 1)
				->setRowStretchFactors(1)
				->put(0, 0, (new GridLayoutElement())
									->setHorizontalSpacing(10)
									->put(0, 0, testItemElement)
									->put(1, 0, testItem2Element)
									->put(2, 0, testItem3Element))
				->setCellSpanning(2, 1)
				->put(2, 0, testNodeItemElement)
				->setCellSpanning(1, 2)
				->put(0, 1, testItem4Element)
				);

	// Test 5: Size dependencies inside the grid & alignment
	addForm((new GridLayoutElement())
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->put(0, 0, (new GridLayoutElement())
									->setHorizontalSpacing(300)
									->put(0, 0, testItemElement)
									->put(1, 0, testItem2Element)))
				->put(0, 1, (new GridLayoutElement())
									->setColumnStretchFactor(1, 1)
									->put(0, 0, testNodeItemElement)
									->put(2, 0, testItem3Element)
									->setCellVerticalAlignment(LayoutStyle::Alignment:: Center))
				->put(1, 0, testItem4Element)
				->setCellSpanning(1, 2);

	// Test 6: Anchor layout
	addForm((new AnchorLayoutElement())
				->put(AnchorLayoutElement::PlaceEdge::TopOf, testItemElement,
						AnchorLayoutElement::AtEdge::AtBottomOf, testItem2Element));
}

int DeclarativeTest::determineForm()
{
	return 6;
}

} /* namespace Visualization */
