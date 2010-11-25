/***********************************************************************************************************************
 * PersistenceTests.cpp
 *
 *  Created on: Nov 11, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "selftest/headers/SelfTestSuite.h"
#include "BinaryNode.h"
#include "BinaryWithPosition.h"
#include "Model.h"
#include "nodes/Integer.h"
#include "nodes/Text.h"
#include "PersistentStoreMock.h"

namespace Model {

TEST(ModelBase, PersistenceSave)
{
	Model model;
	PersistentStoreMock store;

	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	model.save(store);

	CHECK_STR_EQUAL("BinaryNode,root,full,Text,text,full,,Integer,x,full,0,Integer,y,full,0,", store.getSaved());

	model.beginModification(root, "make tree");
	root->makeLeftNode();
	root->makeRightNode();
	root->text()->set("Troot");
	root->left()->text()->set("Tleft");
	root->right()->text()->set("Tright");
	model.endModification();

	store.clear();
	model.save(store);

	CHECK_STR_EQUAL("BinaryNode,root,full,Text,text,full,Troot,BinaryNode,left,full,Text,text,full,Tleft,Integer,x,full,0,Integer,y,full,0,BinaryNode,right,full,Text,text,full,Tright,Integer,x,full,0,Integer,y,full,0,Integer,x,full,0,Integer,y,full,0,", store.getSaved());
}

}
