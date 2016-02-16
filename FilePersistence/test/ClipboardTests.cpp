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

#include "../src/FilePersistencePlugin.h"
#include "../src/simple/SimpleTextFileStore.h"
#include "../src/SystemClipboard.h"
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "ModelBase/src/test_nodes/PartialList.h"
#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Integer.h"
#include "ModelBase/src/nodes/NameText.h"
#include "ModelBase/src/nodes/List.h"

namespace FilePersistence {

class FILEPERSISTENCE_API CopyToClipboard
: public SelfTest::Test<FilePersistencePlugin, CopyToClipboard> { public: void test()
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::TreeManager manager;
	SimpleTextFileStore store;
	store.setBaseFolder(testDir);

	manager.load(&store, "2Children", false);
	SystemClipboard sc;

	TestNodes::BinaryNode* root = DCast<TestNodes::BinaryNode> (manager.root());

	sc.putNode(root);

	QString clipboardText = QApplication::clipboard()->text().simplified();
	CHECK_STR_EQUAL("<!DOCTYPE EnvisionFilePersistence> <clipboard> <BinaryNode name=\"0\"> <NameText name=\"name\">"
			"S_RootNode</NameText>"
			" <BinaryNode name=\"left\"> <NameText name=\"name\">S_Left child</NameText> </BinaryNode>"
			" <BinaryNode name=\"right\"> <NameText name=\"name\">S_Right child</NameText> </BinaryNode>"
			" </BinaryNode> </clipboard>",
			clipboardText);

	QList<const Model::Node*> nodes;
	nodes.append(root->name());
	nodes.append(root->right());
	sc.putNodes(nodes);

	clipboardText = QApplication::clipboard()->text().simplified();
	CHECK_STR_EQUAL("<!DOCTYPE EnvisionFilePersistence> <clipboard> <NameText name=\"0\">S_RootNode</NameText> "
			"<BinaryNode name=\"1\"> <NameText name=\"name\">S_Right child</NameText> </BinaryNode> </clipboard>",
						 clipboardText);
}};

class FILEPERSISTENCE_API CopyPartialToClipboard : public SelfTest::Test<FilePersistencePlugin, CopyPartialToClipboard>
{ public: void test()
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::TreeManager manager;
	SimpleTextFileStore store;
	store.setBaseFolder(testDir);

	manager.load(&store, "partial", false);
	TestNodes::PartialList* root = DCast<TestNodes::PartialList> (manager.root());
	CHECK_CONDITION(root != nullptr);

	SystemClipboard sc;
	sc.putNode(root);

	QString clipboardText = QApplication::clipboard()->text().simplified();
	CHECK_STR_EQUAL("<!DOCTYPE EnvisionFilePersistence> <clipboard> <PartialList name=\"0\"> <List name=\"list\"> "
			"<Text name=\"0\">S_one</Text> <Text name=\"1\">S_two</Text> <Text name=\"2\">S_three</Text> "
			"<Text name=\"3\">S_four</Text> </List> </PartialList> </clipboard>", clipboardText);
}};

class FILEPERSISTENCE_API PasteTextFromClipboard : public SelfTest::Test<FilePersistencePlugin, PasteTextFromClipboard>
{ public: void test()
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::TreeManager manager;
	SimpleTextFileStore store;
	store.setBaseFolder(testDir);

	manager.load(&store, "2Children", false);
	SystemClipboard sc;

	TestNodes::BinaryNode* root = DCast<TestNodes::BinaryNode> (manager.root());

	sc.putNode(root->name());

	bool clipboardDataOK = sc.readClipboard() && sc.numNodes() == 1;
	CHECK_CONDITION(clipboardDataOK);

	CHECK_STR_EQUAL("Left child", root->left()->name()->get());
	manager.beginModification(root->left()->name(), "paste");
	root->left()->name()->load(sc);
	manager.endModification();
	CHECK_STR_EQUAL("RootNode", root->left()->name()->get());
}};

class FILEPERSISTENCE_API PasteBinaryFromClipboard
: public SelfTest::Test<FilePersistencePlugin, PasteBinaryFromClipboard>
{ public: void test()
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::TreeManager manager;
	SimpleTextFileStore store;
	store.setBaseFolder(testDir);

	manager.load(&store, "2Children", false);
	SystemClipboard sc;

	TestNodes::BinaryNode* root = DCast<TestNodes::BinaryNode> (manager.root());

	sc.putNode(root);

	bool clipboardDataOK = sc.readClipboard() && sc.numNodes() == 1;
	CHECK_CONDITION(clipboardDataOK);

	TestNodes::BinaryNode* left = DCast<TestNodes::BinaryNode> (root->left());

	manager.beginModification(left, "paste");
	left->load(sc);
	manager.endModification();

	CHECK_STR_EQUAL("RootNode", left->name()->get());
	CHECK_CONDITION(left->left());
	CHECK_CONDITION(left->right());
	CHECK_STR_EQUAL("Left child", left->left()->name()->get());
	CHECK_STR_EQUAL("Right child", left->right()->name()->get());
}};

class FILEPERSISTENCE_API PasteListFromClipboard : public SelfTest::Test<FilePersistencePlugin, PasteListFromClipboard>
{ public: void test()
{
	auto root = new Model::List;
	Model::TreeManager manager{"test", root};

	manager.beginModification(root, "elems");
	Model::Text* first = new Model::Text{};
	first->set("first");
	root->append(first);
	Model::Text* second = new Model::Text{};
	second->set("second");
	root->append(second);
	Model::List* list = new Model::List{};
	root->append(list);
	Model::Text* third = new Model::Text{};
	third->set("third");
	list->append(third);
	manager.endModification();

	SystemClipboard sc;
	sc.putNode(list);
	sc.readClipboard();

	manager.beginModification(root, "paste");
	root->load(sc);
	manager.endModification();

	CHECK_INT_EQUAL(1, root->size());
	CHECK_STR_EQUAL("third", root->at<Model::Text>(0)->get());
}};

class FILEPERSISTENCE_API PasteInListFromClipboard
 : public SelfTest::Test<FilePersistencePlugin, PasteInListFromClipboard>
{ public: void test()
{
	auto root = new Model::List;
	Model::TreeManager manager{"test", root};

	manager.beginModification(root, "elems");
	Model::Text* first = new Model::Text{};
	first->set("first");
	root->append(first);
	Model::Text* second = new Model::Text{};
	second->set("second");
	root->append(second);
	Model::List* list = new Model::List{};
	root->append(list);
	Model::Text* third = new Model::Text{};
	third->set("third");
	list->append(third);
	manager.endModification();

	SystemClipboard sc;
	QList<const Model::Node*> nodes;
	nodes.append(root->at<Model::Node>(0));
	nodes.append(root->at<Model::Node>(2));
	sc.putNodes(nodes);
	sc.readClipboard();

	manager.beginModification(root, "paste");
	root->paste(sc, 3);
	manager.endModification();

	CHECK_INT_EQUAL(5, root->size());
	CHECK_STR_EQUAL("first", root->at<Model::Text>(3)->get());
	CHECK_INT_EQUAL(1, root->at<Model::List>(4)->size());
	CHECK_STR_EQUAL("third", root->at<Model::List>(4)->at<Model::Text>(0)->get());
}};

}
