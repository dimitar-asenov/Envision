/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "comments.h"
#include "nodes/Comment.h"
#include "nodes/CommentDiagram.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/items/VComposite.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/nodes/TestBoxNode.h"

#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/List.h"
#include "ModelBase/src/model/Model.h"


namespace Comments {

using namespace Visualization;

TEST(Comments, SimpleTest)
{
	auto list = new Model::List();
	auto model = new Model::Model(list);

	model->beginModification(list, "set");
	auto first = new TestNodes::BinaryNode();
	list->append(first);
	auto second = new TestNodes::BinaryNode();
	list->append(second);
	auto third = new Model::Text();
	list->append(third);

	first->name()->set("First node");
	auto left = new TestNodes::BinaryNode();
	first->setLeft(left);
	auto right = new TestNodes::BinaryNode();
	first->setRight(right);
	left->name()->set("left node");
	right->name()->set("right node");

	second->name()->set("Empty node");

	third->set("Some other text");

#if 1
	auto node = new CommentNode(
		"# Header 1\n"
		"Text *in bold* that spans more\n"
		"than one line\n"
		"===\n"
		"\n"
		"## Header 2\n"
		"---\n"
		"\n"
		"...\n"
		"### Header 3\n"
		"#### Header 4\n"
		"###### Header 6\n"
		"And later on, some more text...\n"
		"[[http://www.google.com]]");
#else
	auto node = new CommentDiagram();
	node->shapes()->append(new CommentDiagramShape("First shape",     0,  50, 200,  50, Rectangle));
	node->shapes()->append(new CommentDiagramShape("Another shape", 100, 150, 200,  50, Ellipse));
	node->shapes()->append(new CommentDiagramShape("Diamond",         0, 150, 100, 150, Diamond));

	node->connectors()->append(new CommentDiagramConnector(0, 10, 1, 10));
	node->connectors()->append(new CommentDiagramConnector(1, 4, 2, 4));
#endif

//	auto node = new Comment("This is a node with some [diagram#1] [image#image.png]");
//	node->addDiagram(node);

	list->append(node);

	model->endModification();

	auto top = new RootItem(list);
	auto scene = VisualizationManager::instance().mainScene();
	scene->addTopLevelItem(top);
	QApplication::processEvents();

	scene->scheduleUpdate();
	scene->listenToModel(model);

	CHECK_CONDITION(scene);
}

}
