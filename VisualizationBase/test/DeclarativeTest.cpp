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

namespace Visualization {

ITEM_COMMON_DEFINITIONS(DeclarativeTest, "item")

DeclarativeTest::DeclarativeTest(Item* parent, TestNodes::BinaryNode* node) :
		DeclarativeItem<DeclarativeTest>(parent), testNode_{node}
{
	setPurpose(0);
}

void DeclarativeTest::initializeForms()
{
	// Test VisualizationItemWrapperElement
//	addForm(item<Symbol, I>(&I::testItem_, [](I*){return itemStyles().get();}));

	// Test NodeItemWrapperElement
//	addForm(item<I>(&I::testNodeItemGeneral_, [](I* v){return v->testNode_;}));

	// Test NodeWithVisualizationItemWrapperElement
//	addForm(item<VExtendable,I>(&I::testNodeItem_, [](I* v){return v->testNode_;},
//											[](I*){return VExtendable::itemStyles().get();}));

	// Test GridLayoutElement
	addForm((new GridLayoutElement(3, 2))
				->add(0, 0, item<Symbol, I>(&I::testItem_, [](I*){return itemStyles().get();}))
				->add(0, 1, item<VExtendable,I>(&I::testNodeItem_, [](I* v){return v->testNode_;},
														[](I*){return VExtendable::itemStyles().get();}))
				->add(1, 1, (new GridLayoutElement(2, 2))
								->add(0, 0, item<Symbol, I>(&I::testItem2_, [](I*){return itemStyles().get();}))
								->add(1, 1, item<Symbol, I>(&I::testItem3_, [](I*){return itemStyles().get();})))
				->add(2, 0, item<Symbol, I>(&I::testItem4_, [](I*){return itemStyles().get();})));
}

} /* namespace Visualization */
