/***********************************************************************************************************************
 * ClipboardTests.cpp
 *
 *  Created on: Jan 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"
#include "FileStore.h"
#include "SystemClipboard.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "ModelBase/headers/test_nodes/BinaryNode.h"
#include "ModelBase/headers/test_nodes/PartialList.h"
#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/nodes/Integer.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/List.h"

#include <QtGui/QClipboard>
#include <QtGui/QApplication>

#include <QtCore/QTextStream>
#include <QtCore/QFile>

namespace FilePersistence {

TEST(FilePersistence, CopyToClipboard)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "2Children");
	SystemClipboard sc;

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.root());

	sc.putNode(root);

	QString clipboardText = QApplication::clipboard()->text().simplified();
	CHECK_STR_EQUAL("<!DOCTYPE EnvisionFilePersistence> <clipboard> <BinaryNode name=\"0\" partial=\"0\"> <Text name=\"name\" partial=\"0\">S_Root</Text> <BinaryNode name=\"left\" partial=\"0\"> <Text name=\"name\" partial=\"0\">S_Left child</Text> </BinaryNode> <BinaryNode name=\"right\" partial=\"0\"> <Text name=\"name\" partial=\"0\">S_Right child</Text> </BinaryNode> </BinaryNode> </clipboard>",clipboardText);

	QList<const Model::Node*> nodes;
	nodes.append(root->name());
	nodes.append(root->right());
	sc.putNodes(nodes);

	clipboardText = QApplication::clipboard()->text().simplified();
	CHECK_STR_EQUAL("<!DOCTYPE EnvisionFilePersistence> <clipboard> <Text name=\"0\" partial=\"0\">S_Root</Text> <BinaryNode name=\"1\" partial=\"0\"> <Text name=\"name\" partial=\"0\">S_Right child</Text> </BinaryNode> </clipboard>",clipboardText);
}

TEST(FilePersistence, CopyPartialToClipboard)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "partial");
	TestNodes::PartialList* root = dynamic_cast<TestNodes::PartialList*> (model.root());
	CHECK_CONDITION(root != NULL);

	SystemClipboard sc;
	sc.putNode(root);

	QString clipboardText = QApplication::clipboard()->text().simplified();
	CHECK_STR_EQUAL("<!DOCTYPE EnvisionFilePersistence> <clipboard> <PartialList name=\"0\" partial=\"0\"> <List name=\"list\" partial=\"0\"> <Text name=\"ModelBaseListRefName\" partial=\"0\">S_TheList</Text> <Text name=\"0\" partial=\"0\">S_one</Text> <Text name=\"1\" partial=\"0\">S_two</Text> <Text name=\"2\" partial=\"0\">S_three</Text> <Text name=\"3\" partial=\"0\">S_four</Text> </List> </PartialList> </clipboard>",clipboardText);
}

TEST(FilePersistence, PasteTextFromClipboard)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "2Children");
	SystemClipboard sc;

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.root());

	sc.putNode(root->name());

	bool clipboardDataOK = sc.readClipboard() && sc.numNodes() == 1;
	CHECK_CONDITION(clipboardDataOK);

	CHECK_STR_EQUAL("Left child", root->left()->name()->get());
	model.beginModification(root->left()->name(), "paste");
	root->left()->name()->load(sc);
	model.endModification();
	CHECK_STR_EQUAL("Root", root->left()->name()->get());
}

TEST(FilePersistence, PasteBinaryFromClipboard)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "2Children");
	SystemClipboard sc;

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.root());

	sc.putNode(root);

	bool clipboardDataOK = sc.readClipboard() && sc.numNodes() == 1;
	CHECK_CONDITION(clipboardDataOK);

	TestNodes::BinaryNode* left = dynamic_cast<TestNodes::BinaryNode*> (root->left());

	model.beginModification(left, "paste");
	left->load(sc);
	model.endModification();

	CHECK_STR_EQUAL("Root", left->name()->get());
	CHECK_CONDITION(left->left());
	CHECK_CONDITION(left->right());
	CHECK_INT_EQUAL(6, left->name()->getId());
	CHECK_INT_EQUAL(7, left->left()->getId());
	CHECK_INT_EQUAL(8, left->left()->name()->getId());
	CHECK_INT_EQUAL(9, left->right()->getId());
	CHECK_INT_EQUAL(10, left->right()->name()->getId());
	CHECK_STR_EQUAL("Left child", left->left()->name()->get());
	CHECK_STR_EQUAL("Right child", left->right()->name()->get());
}

TEST(FilePersistence, PasteListFromClipboard)
{
	Model::Model model;
	model.setName("test");

	Model::List* root = dynamic_cast<Model::List*> (model.createRoot("List"));

	model.beginModification(root, "elems");
	root->setReferenceName("Hello");
	root->append<Model::Text>()->set("first");
	root->append<Model::Text>()->set("second");
	Model::List* list = root->append<Model::List>();
	list->setReferenceName("Under");
	list->append<Model::Text>()->set("third");
	model.endModification();

	SystemClipboard sc;
	sc.putNode(list);
	sc.readClipboard();

	model.beginModification(root, "paste");
	root->load(sc);
	model.endModification();

	CHECK_STR_EQUAL("Under", root->getReferenceName());
	CHECK_INT_EQUAL(1, root->size());
	CHECK_INT_EQUAL(0, root->getId());
	CHECK_INT_EQUAL(8,root->at<Model::Node>(0)->getId());
	CHECK_STR_EQUAL("third",root->at<Model::Text>(0)->get());
}

TEST(FilePersistence, PasteInListFromClipboard)
{
	Model::Model model;
	model.setName("test");

	Model::List* root = dynamic_cast<Model::List*> (model.createRoot("List"));

	model.beginModification(root, "elems");
	root->setReferenceName("Hello");
	root->append<Model::Text>()->set("first");
	root->append<Model::Text>()->set("second");
	Model::List* list = root->append<Model::List>();
	list->setReferenceName("Under");
	list->append<Model::Text>()->set("third");
	model.endModification();

	SystemClipboard sc;
	QList<const Model::Node*> nodes;
	nodes.append(root->at<Model::Node>(0));
	nodes.append(root->at<Model::Node>(2));
	sc.putNodes(nodes);
	sc.readClipboard();

	model.beginModification(root, "paste");
	root->paste(sc,3);
	model.endModification();

	CHECK_INT_EQUAL(5, root->size());
	CHECK_INT_EQUAL(0, root->getId());
	CHECK_INT_EQUAL(7, root->at<Model::Node>(3)->getId());
	CHECK_INT_EQUAL(8, root->at<Model::Node>(4)->getId());
	CHECK_STR_EQUAL("first",root->at<Model::Text>(3)->get());
	CHECK_INT_EQUAL(1, root->at<Model::List>(4)->size());
	CHECK_STR_EQUAL("third",root->at<Model::List>(4)->at<Model::Text>(0)->get());
}

}
