/***********************************************************************************************************************
 * ListTest.cpp
 *
 *  Created on: Dec 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "BinaryNode.h"
#include "BinaryNodeUnit.h"
#include "Model.h"
#include "nodes/Text.h"
#include "nodes/Integer.h"
#include "nodes/Reference.h"
#include "nodes/List.h"

namespace Model {

TEST(ModelBase, ListCreation)
{
	Model model;
	List* root = dynamic_cast<List*> (model.createRoot("List"));

	CHECK_INT_EQUAL(0, root->size());

	model.beginModification(root, "add elements to list");
	Text* a = dynamic_cast<Text*> (root->append("Text"));
	Text* b = dynamic_cast<Text*> (root->append("Text"));
	Integer* c = dynamic_cast<Integer*> (root->append("Integer"));
	a->set("first");
	b->set("second");
	c->set(3);
	model.endModification();

	CHECK_INT_EQUAL(3, root->size());
	CHECK_CONDITION((*root)[0] == a);
	CHECK_CONDITION((*root)[1] == b);
	CHECK_CONDITION((*root)[2] == c);
	CHECK_CONDITION(a != NULL);
	CHECK_CONDITION(b != NULL);
	CHECK_CONDITION(c != NULL);
}

TEST(ModelBase, ListInsertion)
{
	Model model;
	List* root = dynamic_cast<List*> (model.createRoot("List"));

	model.beginModification(root, "add elements to list");
	Text* a = dynamic_cast<Text*> (root->append("Text"));
	Text* b = dynamic_cast<Text*> (root->prepend("Text"));
	Text* c = dynamic_cast<Text*> (root->append("Text"));
	Text* d = dynamic_cast<Text*> (root->prepend("Text"));
	Text* e = dynamic_cast<Text*> (root->insert("Text",2));
	Text* f = dynamic_cast<Text*> (root->insert("Text",3));
	Text* g = dynamic_cast<Text*> (root->insert("Text",1));
	model.endModification();

	CHECK_INT_EQUAL(7, root->size());
	CHECK_CONDITION(root->first() == d);
	CHECK_CONDITION((*root)[1] == g);
	CHECK_CONDITION((*root)[2] == b);
	CHECK_CONDITION((*root)[3] == e);
	CHECK_CONDITION((*root)[4] == f);
	CHECK_CONDITION((*root)[5] == a);
	CHECK_CONDITION(root->last() == c);
}

TEST(ModelBase, ListRemoval)
{
	Model model;
	List* root = dynamic_cast<List*> (model.createRoot("List"));

	model.beginModification(root, "add elements to list");
	Text* a = dynamic_cast<Text*> (root->append("Text"));
	Text* b = dynamic_cast<Text*> (root->append("Text"));
	Text* c = dynamic_cast<Text*> (root->append("Text"));
	Text* d = dynamic_cast<Text*> (root->append("Text"));
	Text* e = dynamic_cast<Text*> (root->append("Text"));
	Text* f = dynamic_cast<Text*> (root->append("Text"));
	Text* g = dynamic_cast<Text*> (root->append("Text"));
	model.endModification();

	model.beginModification(root, "remove elements from list");
	root->remove(3);
	root->remove(a);
	root->remove(4);
	root->remove(b);
	model.endModification();

	CHECK_CONDITION(d != NULL);
	CHECK_CONDITION(g != NULL);

	CHECK_INT_EQUAL(3, root->size());
	CHECK_CONDITION(root->first() == c);
	CHECK_CONDITION((*root)[1] == e);
	CHECK_CONDITION(root->last() == f);
}

TEST(ModelBase, ListUndo)
{
	Model model;
	List* root = dynamic_cast<List*> (model.createRoot("List"));

	model.beginModification(root, "add elements to list");
	Text* a = dynamic_cast<Text*> (root->append("Text"));
	Text* b = dynamic_cast<Text*> (root->append("Text"));
	Text* c = dynamic_cast<Text*> (root->append("Text"));
	Text* d = dynamic_cast<Text*> (root->append("Text"));
	Text* e = dynamic_cast<Text*> (root->append("Text"));
	Text* f = dynamic_cast<Text*> (root->append("Text"));
	Text* g = dynamic_cast<Text*> (root->append("Text"));
	model.endModification();

	CHECK_INT_EQUAL(7, root->size());
	CHECK_CONDITION((*root)[0] == a);
	CHECK_CONDITION((*root)[1] == b);
	CHECK_CONDITION((*root)[2] == c);
	CHECK_CONDITION((*root)[3] == d);
	CHECK_CONDITION((*root)[4] == e);
	CHECK_CONDITION((*root)[5] == f);
	CHECK_CONDITION((*root)[6] == g);

	model.beginModification(NULL);
	model.undo();
	model.endModification();

	CHECK_INT_EQUAL(0, root->size());

	model.beginModification(NULL);
	model.redo();
	model.endModification();

	CHECK_INT_EQUAL(7, root->size());
	CHECK_CONDITION((*root)[0] == a);
	CHECK_CONDITION((*root)[1] == b);
	CHECK_CONDITION((*root)[2] == c);
	CHECK_CONDITION((*root)[3] == d);
	CHECK_CONDITION((*root)[4] == e);
	CHECK_CONDITION((*root)[5] == f);
	CHECK_CONDITION((*root)[6] == g);

	model.beginModification(root, "add elements to list");
	root->remove(a);
	root->remove(e);
	model.endModification();

	CHECK_INT_EQUAL(5, root->size());
	CHECK_CONDITION((*root)[0] == b);
	CHECK_CONDITION((*root)[1] == c);
	CHECK_CONDITION((*root)[2] == d);
	CHECK_CONDITION((*root)[3] == f);
	CHECK_CONDITION((*root)[4] == g);

	model.beginModification(NULL);
	model.undo();
	model.endModification();

	CHECK_INT_EQUAL(7, root->size());
	CHECK_CONDITION((*root)[0] == a);
	CHECK_CONDITION((*root)[1] == b);
	CHECK_CONDITION((*root)[2] == c);
	CHECK_CONDITION((*root)[3] == d);
	CHECK_CONDITION((*root)[4] == e);
	CHECK_CONDITION((*root)[5] == f);
	CHECK_CONDITION((*root)[6] == g)

	model.beginModification(NULL);
	model.redo();
	model.endModification();

	CHECK_INT_EQUAL(5, root->size());
	CHECK_CONDITION((*root)[0] == b);
	CHECK_CONDITION((*root)[1] == c);
	CHECK_CONDITION((*root)[2] == d);
	CHECK_CONDITION((*root)[3] == f);
	CHECK_CONDITION((*root)[4] == g);

}

}
