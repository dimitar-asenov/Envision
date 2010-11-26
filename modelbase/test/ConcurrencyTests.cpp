/***********************************************************************************************************************
 * ConcurrencyTests.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "selftest/headers/SelfTestSuite.h"
#include "BinaryNode.h"
#include "BinaryWithPosition.h"
#include "BinaryNodeUnit.h"
#include "Model.h"
#include "nodes/Integer.h"
#include "nodes/Text.h"

namespace Model {

//TEST(ModelBase, ConcurrentRead)
//{
//	Model model;
//
//	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));
//
//	model.beginModification(root, "make tree");
//	root->makeLeftNode();
//	root->makeRightNode();
//	root->text()->set("Troot");
//	root->left()->text()->set("Tleft");
//	root->right()->text()->set("Tright");
//	model.endModification();
//
//}

}
