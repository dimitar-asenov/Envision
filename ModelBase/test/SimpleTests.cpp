/***********************************************************************************************************************
 * SimpleTests.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "BinaryNode.h"
#include "BinaryNodeUnit.h"
#include "Model.h"
#include "nodes/Text.h"

namespace Model {

TEST(ModelBase, ExtendableMetaData)
{
	AttributeChain& metaExt = ExtendableNode::getMetaData<BinaryNode>();
	AttributeChain& metaUnit = ExtendableNode::getMetaData<BinaryNodeUnit>();

	CHECK_INT_EQUAL(1, metaExt.getNumLevels());
	CHECK_INT_EQUAL(2, metaUnit.getNumLevels());

	CHECK_INT_EQUAL(5, metaExt.size());
	CHECK_INT_EQUAL(0, metaUnit.size());
	CHECK_CONDITION( metaUnit.getLevel(0) == &metaExt);

	CHECK_STR_EQUAL("text", metaExt[0].name());
	CHECK_STR_EQUAL("left", metaExt[1].name());
	CHECK_STR_EQUAL("right", metaExt[2].name());
	CHECK_STR_EQUAL("x", metaExt[3].name());
	CHECK_STR_EQUAL("y", metaExt[4].name());

	CHECK_STR_EQUAL("Text", metaExt[0].type());
	CHECK_STR_EQUAL("BinaryNode", metaExt[1].type());
	CHECK_STR_EQUAL("BinaryNode", metaExt[2].type());
	CHECK_STR_EQUAL("Integer", metaExt[3].type());
	CHECK_STR_EQUAL("Integer", metaExt[4].type());

	CHECK_CONDITION(metaExt[0].optional() == false);
	CHECK_CONDITION(metaExt[1].optional() == true);
	CHECK_CONDITION(metaExt[2].optional() == true);
	CHECK_CONDITION(metaExt[3].optional() == false);
	CHECK_CONDITION(metaExt[4].optional() == false);

	CHECK_CONDITION(metaExt[0].partialHint() == false);
	CHECK_CONDITION(metaExt[1].partialHint() == false);
	CHECK_CONDITION(metaExt[2].partialHint() == false);
	CHECK_CONDITION(metaExt[3].partialHint() == false);
	CHECK_CONDITION(metaExt[4].partialHint() == false);
}

TEST(ModelBase, SimpleModelCreation)
{
	Model model;
	CHECK_CONDITION( model.getRoot() == NULL );

	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));
	CHECK_CONDITION( model.getRoot() == root );

	CHECK_CONDITION( root->getModel() == &model );

	CHECK_CONDITION( root->text()->getModel() == &model );
}

TEST(ModelBase, RemoveOptional)
{
	Model model;
	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "Making left node");
	BinaryNode* left = root->makeLeftNode();
	model.endModification();
	CHECK_CONDITION( root->left() == left );
	CHECK_CONDITION( root->left() != NULL );

	model.beginModification(root, "Removing left node");
	root->removeLeftNode();
	model.endModification();
	CHECK_CONDITION( root->left() == NULL);

	model.beginModification(root, "Making left node");
	root->makeLeftNode();
	model.endModification();
	CHECK_CONDITION( root->left() != left );
	CHECK_CONDITION( root->left() != NULL );
}

TEST(ModelBase, ChildNodeRetrieval)
{
	Model model;
	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "Making nodes");
	BinaryNode* left = root->makeLeftNode();
	BinaryNode* right = root->makeRightNode();
	model.endModification();

	CHECK_INT_EQUAL(0, root->getId());
	CHECK_INT_EQUAL(4, left->getId());
	CHECK_INT_EQUAL(8, right->getId());

	CHECK_CONDITION(root->getChild(1) == root->text());
	CHECK_CONDITION(root->getChild(4) == left);
	CHECK_CONDITION(root->getChild(8) == right);

	CHECK_CONDITION(root->hasAttribute("text"));
	CHECK_CONDITION(root->hasAttribute("left"));
	CHECK_CONDITION(root->hasAttribute("right"));

	CHECK_CONDITION(root->hasAttribute("another") == false);

	CHECK_CONDITION(root->get("text") == root->text());
	CHECK_CONDITION(root->get("left") == left);
	CHECK_CONDITION(root->get("right") == right);
}

}
