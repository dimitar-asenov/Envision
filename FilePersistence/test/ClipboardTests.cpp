/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "filepersistence.h"
#include "FileStore.h"
#include "SystemClipboard.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "ModelBase/src/test_nodes/PartialList.h"
#include "ModelBase/src/model/Model.h"
#include "ModelBase/src/nodes/Integer.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/List.h"

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
	CHECK_CONDITION(root != nullptr);

	SystemClipboard sc;
	sc.putNode(root);

	QString clipboardText = QApplication::clipboard()->text().simplified();
	CHECK_STR_EQUAL("<!DOCTYPE EnvisionFilePersistence> <clipboard> <PartialList name=\"0\" partial=\"0\"> <List name=\"list\" partial=\"0\"> <Text name=\"0\" partial=\"0\">S_one</Text> <Text name=\"1\" partial=\"0\">S_two</Text> <Text name=\"2\" partial=\"0\">S_three</Text> <Text name=\"3\" partial=\"0\">S_four</Text> </List> </PartialList> </clipboard>",clipboardText);
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
	CHECK_STR_EQUAL("Left child", left->left()->name()->get());
	CHECK_STR_EQUAL("Right child", left->right()->name()->get());
}

TEST(FilePersistence, PasteListFromClipboard)
{
	auto root = new Model::List;
	Model::Model model("test", root);

	model.beginModification(root, "elems");
	Model::Text* first = new Model::Text();
	first->set("first");
	root->append(first);
	Model::Text* second = new Model::Text();
	second->set("second");
	root->append(second);
	Model::List* list = new Model::List();
	root->append(list);
	Model::Text* third = new Model::Text();
	third->set("third");
	list->append(third);
	model.endModification();

	SystemClipboard sc;
	sc.putNode(list);
	sc.readClipboard();

	model.beginModification(root, "paste");
	root->load(sc);
	model.endModification();

	CHECK_INT_EQUAL(1, root->size());
	CHECK_STR_EQUAL("third",root->at<Model::Text>(0)->get());
}

TEST(FilePersistence, PasteInListFromClipboard)
{
	auto root = new Model::List;
	Model::Model model("test",root);

	model.beginModification(root, "elems");
	Model::Text* first = new Model::Text();
	first->set("first");
	root->append(first);
	Model::Text* second = new Model::Text();
	second->set("second");
	root->append(second);
	Model::List* list = new Model::List();
	root->append(list);
	Model::Text* third = new Model::Text();
	third->set("third");
	list->append(third);
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
	CHECK_STR_EQUAL("first",root->at<Model::Text>(3)->get());
	CHECK_INT_EQUAL(1, root->at<Model::List>(4)->size());
	CHECK_STR_EQUAL("third",root->at<Model::List>(4)->at<Model::Text>(0)->get());
}

}
