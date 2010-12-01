/***********************************************************************************************************************
 * PositionTests.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "selftest/headers/SelfTestSuite.h"
#include "BinaryNode.h"
#include "BinaryWithPosition.h"
#include "Model.h"
#include "nodes/Integer.h"

namespace Model {

TEST(ModelBase, PositionExtension)
{
	Model model;
	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	CHECK_CONDITION(root->hasAttribute("x"));
	CHECK_CONDITION(root->hasAttribute("y"));

	BinaryWithPosition pos(root);

	CHECK_CONDITION( pos.binary() == root );
	CHECK_INT_EQUAL( 0, pos.x() );
	CHECK_INT_EQUAL( 0, pos.y() );

	model.beginModification(root, "Setting values");
	pos.set(42, -18);
	model.endModification();

	CHECK_INT_EQUAL( 42, pos.x() );
	CHECK_INT_EQUAL( -18, pos.y() );
}

}
