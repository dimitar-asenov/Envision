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

#include "../src/ModelBasePlugin.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "../src/test_nodes/BinaryNode.h"
#include "../src/test_nodes/PositionExtension.h"
#include "../src/model/TreeManager.h"
#include "../src/nodes/Integer.h"

namespace Model {

class MODELBASE_API PositionExtension : public Test<ModelBasePlugin, PositionExtension> { public: void test()
{
	auto root = new TestNodes::BinaryNode{};
	TreeManager manager{root};

	CHECK_CONDITION(root->hasAttribute("_ext_PositionExtension_x"));
	CHECK_CONDITION(root->hasAttribute("_ext_PositionExtension_y"));

	auto pos = root->extension<TestNodes::PositionExtension>();

	CHECK_INT_EQUAL( 0, pos->x() );
	CHECK_INT_EQUAL( 0, pos->y() );

	manager.beginModification(root, "Setting values");
	pos->setX(42);
	pos->setY(-18);
	manager.endModification();

	CHECK_INT_EQUAL( 42, pos->x() );
	CHECK_INT_EQUAL( -18, pos->y() );
}};

}
