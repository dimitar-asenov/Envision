/***********************************************************************************************************************
 * SimpleTests.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "test_nodes/BinaryNodeAccessUnit.h"
#include "Model.h"
#include "nodes/Text.h"
#include "nodes/Integer.h"
#include "nodes/Reference.h"

namespace Model {

TEST(ModelBase, ExtendableMetaData)
{
	AttributeChain& metaExt = ExtendableNode::getMetaData<TestNodes::BinaryNode>();
	AttributeChain& metaUnit = ExtendableNode::getMetaData<TestNodes::BinaryNodeAccessUnit>();

	CHECK_INT_EQUAL(1, metaExt.getNumLevels());
	CHECK_INT_EQUAL(2, metaUnit.getNumLevels());

	CHECK_INT_EQUAL(5, metaExt.size());
	CHECK_INT_EQUAL(0, metaUnit.size());
	CHECK_CONDITION( metaUnit.getLevel(0) == &metaExt);

	CHECK_STR_EQUAL("name", metaExt[0].name());
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

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));
	CHECK_CONDITION( model.getRoot() == root );

	CHECK_CONDITION( root->getModel() == &model );

	CHECK_CONDITION( root->name()->getModel() == &model );
}

TEST(ModelBase, RemoveOptional)
{
	Model model;
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "Making left node");
	TestNodes::BinaryNode* left = root->makeLeftNode();
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
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "Making nodes");
	TestNodes::BinaryNode* left = root->makeLeftNode();
	TestNodes::BinaryNode* right = root->makeRightNode();
	model.endModification();

	CHECK_INT_EQUAL(0, root->getId());
	CHECK_INT_EQUAL(4, left->getId());
	CHECK_INT_EQUAL(8, right->getId());

	CHECK_CONDITION(root->getChild(1) == root->name());
	CHECK_CONDITION(root->getChild(4) == left);
	CHECK_CONDITION(root->getChild(8) == right);

	CHECK_CONDITION(root->hasAttribute("name"));
	CHECK_CONDITION(root->hasAttribute("left"));
	CHECK_CONDITION(root->hasAttribute("right"));

	CHECK_CONDITION(root->hasAttribute("another") == false);

	CHECK_CONDITION(root->get("name") == root->name());
	CHECK_CONDITION(root->get("left") == left);
	CHECK_CONDITION(root->get("right") == right);
}

TEST(ModelBase, ProperRegistration)
{
	Model model;
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));
	CHECK_CONDITION(root->getTypeId() >= 0);

	Model model2;
	Text* t = dynamic_cast<Text*> (model2.createRoot("Text"));
	CHECK_CONDITION(t->getTypeId() >= 0);
	CHECK_CONDITION(t->getTypeId() != root->getTypeId());

	Model model3;
	Integer* i = dynamic_cast<Integer*> (model3.createRoot("Integer"));
	CHECK_CONDITION(i->getTypeId() >= 0);
	CHECK_CONDITION(i->getTypeId() != root->getTypeId());
	CHECK_CONDITION(i->getTypeId() != t->getTypeId());

	Model model4;
	Reference* r = dynamic_cast<Reference*> (model4.createRoot("Reference"));
	CHECK_CONDITION(r->getTypeId() >= 0);
	CHECK_CONDITION(r->getTypeId() != root->getTypeId());
	CHECK_CONDITION(r->getTypeId() != t->getTypeId());
	CHECK_CONDITION(r->getTypeId() != i->getTypeId());
}

}
