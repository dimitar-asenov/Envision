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

#include "interactionbase.h"
#include "HBinaryNode.h"
#include "../src/autocomplete/AutoComplete.h"

#include "SelfTest/src/SelfTestSuite.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/VExtendable.h"
#include "VisualizationBase/src/items/RootItem.h"

#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/List.h"
#include "ModelBase/src/model/Model.h"

namespace Interaction {

using namespace Visualization;

TEST(InteractionBase, TextSelect)
{
	Visualization::VExtendable::setDefaultClassHandler(HBinaryNode::instance());

	Model::Model* model = new Model::Model();
	Model::List* list = static_cast<Model::List*> (model->createRoot("List"));

	model->beginModification(list, "set");
	TestNodes::BinaryNode* first = new TestNodes::BinaryNode();
	list->append(first);
	TestNodes::BinaryNode* second = new TestNodes::BinaryNode();
	list->append(second);
	Model::Text* third = new Model::Text();
	list->append(third);

	first->name()->set("First node");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode();
	first->setLeft(left);
	TestNodes::BinaryNode* right = new TestNodes::BinaryNode();
	first->setRight(right);
	left->name()->set("left node");
	right->name()->set("right node");

	second->name()->set("Empty node");

	third->set("Some independent text");
	model->endModification();

	auto top = new RootItem(list);
	auto scene = VisualizationManager::instance().mainScene();
	scene->addTopLevelItem( top );
	QApplication::processEvents();

	VList* l = dynamic_cast<VList*> (top->item());
	l->itemAt<VExtendable>(0)->setExpanded();
	scene->scheduleUpdate();
	scene->listenToModel(model);

	CHECK_CONDITION(scene);
}

}
