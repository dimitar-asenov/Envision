/***********************************************************************************************************************
 * PositionTests.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "test_nodes/PositionExtension.h"
#include "Model.h"
#include "nodes/Integer.h"

namespace Model {

TEST(ModelBase, PositionExtension)
{
	Model model;
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	CHECK_CONDITION(root->hasAttribute("_ext_PositionExtension_x"));
	CHECK_CONDITION(root->hasAttribute("_ext_PositionExtension_y"));

	TestNodes::PositionExtension* pos = root->extension<TestNodes::PositionExtension>();

	CHECK_INT_EQUAL( 0, pos->x() );
	CHECK_INT_EQUAL( 0, pos->y() );

	model.beginModification(root, "Setting values");
	pos->setX(42);
	pos->setY(-18);
	model.endModification();

	CHECK_INT_EQUAL( 42, pos->x() );
	CHECK_INT_EQUAL( -18, pos->y() );
}

}
