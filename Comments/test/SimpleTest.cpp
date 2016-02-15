/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "../src/CommentsPlugin.h"
#include "../src/nodes/CommentNode.h"
#include "../src/nodes/CommentDiagram.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/items/VComposite.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/nodes/TestBoxNode.h"
#include "OOModel/src/allOOModelNodes.h"

#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/List.h"
#include "ModelBase/src/model/TreeManager.h"

using namespace OOModel;
using namespace Visualization;

namespace Comments {

class SimpleTest : public SelfTest::Test<CommentsPlugin, SimpleTest> { public: void test()
{
	auto list = new Model::List{};
	auto manager = new Model::TreeManager{list};

	manager->beginModification(list, "set");

	auto node = new CommentNode{
		"Comments support *quite* some stuff by **now**. For example...\n"

		"# Diagrams!\n"
		"[diagram#main]\n"
		"And again\n"
		"[diagram#main]\n"

		"# Sourcecode!\n"
		"[code#aCode]\n"

		"# Tables!\n"
		"[table#aTable#3#4]\n"
		"And again\n"
		"[table#aTable]\n"

		"Also, lists work...\n"
		" * A first item\n"
		" * A second item\n"
		"   This one spans two lines\n"
		" * And so on, of course...\n"

		"# Images\n"
		"[image#styles/icon/loop.svg|20x]\n"
		"...\n"

		"# Browsers\n"
		"[browser#http://dimitar-asenov.github.io/Envision/|600x400]\n"
		"...\n"

		"# Inline HTML to Browser\n"
		"<html>\n"
		"	<script type=\"text/javascript\">\n"
		"		function hi() { alert(\"Hello World!\"); } \n"
		"	</script>\n"
		"	<button onclick=\"hi()\">Try it</button>\n"
		"</html>"

	};

	auto diagram = new CommentDiagram{nullptr, "main"};

	auto shape1 = new CommentDiagramShape{0, 50, 200, 50, CommentDiagramShape::ShapeType::Rectangle};
	shape1->setLabel(new CommentText{"First shape"});
	shape1->setShapeColor("blue");
	shape1->setTextColor("red");
	diagram->shapes()->append(shape1);

	auto shape2 = new CommentDiagramShape{100, 150, 200,  50, CommentDiagramShape::ShapeType::Ellipse};
	shape2->setLabel(new CommentText{"Another shape"});
	shape2->setShapeColor("magenta");
	shape2->setTextColor("yellow");
	diagram->shapes()->append(shape2);

	auto shape3 = new CommentDiagramShape{0, 150, 100, 150, CommentDiagramShape::ShapeType::Diamond};
	shape3->setLabel(new CommentText{"Diamond"});
	diagram->shapes()->append(shape3);

	diagram->connectors()->append(new CommentDiagramConnector{0, 6, 1, 0});
	diagram->connectors()->append(new CommentDiagramConnector{1, 0,  2, 0});
	diagram->connectors()->append(new CommentDiagramConnector{2, 0,  0, 10});

	node->diagrams()->append(diagram);

	auto code = new CommentFreeNode{nullptr, "aCode"};
	auto aClass = new Class{"HelloWorld"};
	code->setNode(aClass);
	node->codes()->append(code);

	auto table = new CommentTable{nullptr, "aTable", 3, 4};
	table->setNodeAt(0, 0, new CommentNode{"##Column 1"});
	table->setNodeAt(0, 1, new CommentNode{"##Column 2"});
	table->setNodeAt(0, 2, new CommentNode{"##Column 3"});
	table->setNodeAt(0, 3, new CommentNode{"##Column 4"});
	table->setNodeAt(1, 0, new Class{"ClassA"});
	table->setNodeAt(1, 1, new Class{"ClassB"});
	table->setNodeAt(1, 2, new Class{"ClassC"});
	table->setNodeAt(1, 3, new Class{"ClassD"});
	table->setNodeAt(2, 0, new CommentText{"just text"});
	table->setNodeAt(2, 1, new CommentText{"more text"});
	node->tables()->append(table);

	list->append(node);

	manager->endModification();

	auto scene = VisualizationManager::instance().mainScene();
	scene->addTopLevelNode(list);
	QApplication::processEvents();

	scene->scheduleUpdate();
	scene->listenToTreeManager(manager);

	CHECK_CONDITION(scene);
}};

}
