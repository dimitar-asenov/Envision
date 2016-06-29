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
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

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

	/////////////////////////////////////////////////////////////////////////////
	/// Rich text, lists
	/////////////////////////////////////////////////////////////////////////////
	auto richText = new CommentNode{
		"Envision's comments support a lot *of features*:\n"
		" * **Rich** <font color=\"red\">text</font>.\n"
		" * Markdown syntax\n"
		" * Lists\n"
		"   also spanning multiple lines\n"
		" * And more (see the headings below)"
	};
	list->append(richText);
	list->append(new Model::Text(""));

	/////////////////////////////////////////////////////////////////////////////
	/// Diagrams
	/////////////////////////////////////////////////////////////////////////////
	auto diagramComment = new CommentNode{
		"# Diagrams!\n"
		"A diagram can be drawn directly in the code\n"
		"[diagram#main]\n"
		"The same diagram might be referenced in other locations\n"
		"in the comment. The two diagrams are backed by the same\n"
		"model object and updates are synchronized.\n"
		"[diagram#main]"
	};
	list->append(diagramComment);
	list->append(new Model::Text(""));

	auto diagram = new CommentDiagram{nullptr, "main"};

	auto shape1 = new CommentDiagramShape{50, 10, 60, 50, CommentDiagramShape::ShapeType::Rectangle};
	shape1->setLabel(new CommentText{"Model"});
	shape1->setOutlineType(Qt::NoPen);
	shape1->setBackgroundColor("#a3c0f2");
	shape1->setTextColor("colors/DarkCornflowerBlue3");
	diagram->shapes()->append(shape1);

	auto shape2 = new CommentDiagramShape{10, 100, 60, 50, CommentDiagramShape::ShapeType::Ellipse};
	shape2->setLabel(new CommentText{"View"});
	shape2->setOutlineType(Qt::NoPen);
	shape2->setBackgroundColor("#f4b16b");
	shape2->setTextColor("colors/DarkOrange2");
	diagram->shapes()->append(shape2);

	auto shape3 = new CommentDiagramShape{100, 100, 90,  50, CommentDiagramShape::ShapeType::Ellipse};
	shape3->setLabel(new CommentText{"Controller"});
	shape3->setShapeColor("#990000");
	shape3->setOutlineSize(3);
	shape3->setOutlineType(Qt::DashLine);
	shape3->setBackgroundColor("#f4b16b");
	shape3->setTextColor("colors/DarkOrange2");
	diagram->shapes()->append(shape3);

	diagram->connectors()->append(new CommentDiagramConnector{0, 8, 1, 0});
	diagram->connectors()->last()->setEndArrow(true);
	diagram->connectors()->append(new CommentDiagramConnector{0, 8,  2, 0});
	diagram->connectors()->last()->setEndArrow(true);
	diagram->connectors()->append(new CommentDiagramConnector{1, 4,  2, 12});

	diagramComment->diagrams()->append(diagram);

	/////////////////////////////////////////////////////////////////////////////
	/// Tables and embedded source code
	/////////////////////////////////////////////////////////////////////////////
	auto structuresComment = new CommentNode{
		"# Sourcecode!\n"
		"[code#aCode]\n"

		"# Tables!\n"
		"[table#aTable#3#4]"
	};
	list->append(structuresComment);
	list->append(new Model::Text(""));

	auto code = new CommentFreeNode{nullptr, "aCode"};
	auto aBlock = new Block{QList<StatementItem*>{new ExpressionStatement{new VariableDeclarationExpression{"id",
		new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT},
		new MethodCallExpression{"getId"}}}}};

	code->setNode(aBlock);
	structuresComment->codes()->append(code);

	auto table = new CommentTable{nullptr, "aTable", 3, 3};
	table->setNodeAt(0, 0, new CommentNode{"##Column 1"});
	table->setNodeAt(0, 1, new CommentNode{"##Column 2"});
	table->setNodeAt(0, 2, new CommentNode{"##Column 3"});
	table->setNodeAt(1, 0, new Class{"ClassA"});
	table->setNodeAt(1, 1, new IfStatement{});
	table->setNodeAt(2, 0, new CommentText{"some text"});
	structuresComment->tables()->append(table);

	/////////////////////////////////////////////////////////////////////////////
	/// Images
	/////////////////////////////////////////////////////////////////////////////
	auto imageComment = new CommentNode{
		"# Images\n"
		"[image#styles/icon/loop.svg|20x]\n"
		"Images can be resized with Shift + Right click & Drag."
	};
	list->append(imageComment);
	list->append(new Model::Text(""));

	/////////////////////////////////////////////////////////////////////////////
	/// Web Browsers
	/////////////////////////////////////////////////////////////////////////////
	auto browserComment = new CommentNode{
		"# Browsers\n"
		"[browser#https://en.wikipedia.org/wiki/Quicksort|800x400]\n"
		"...\n"

		"# Inline HTML to Browser\n"
		"<html>\n"
		"	<script type=\"text/javascript\">\n"
		"		function hi() { alert(\"Hello World!\"); } \n"
		"	</script>\n"
		"	<button onclick=\"hi()\">Try it</button>\n"
		"</html>"

	};
	list->append(browserComment);

	manager->endModification();

	auto scene = VisualizationManager::instance().mainScene();
	scene->addTopLevelNode(list);
	QApplication::processEvents();

	scene->scheduleUpdate();
	scene->listenToTreeManager(manager);

	CHECK_CONDITION(scene);
}};

}
