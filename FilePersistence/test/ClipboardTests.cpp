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
#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/nodes/Integer.h"
#include "ModelBase/headers/nodes/Text.h"

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

	model.load(store, "2Children");
	SystemClipboard sc;

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.getRoot());

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

}
