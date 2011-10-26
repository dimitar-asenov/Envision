/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "interactionbase.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/views/MainView.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/items/VExtendable.h"

#include "ModelBase/headers/test_nodes/BinaryNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/Model.h"

#include <QtGui/QApplication>

namespace Interaction {

using namespace Visualization;

TEST(InteractionBase, TextSelect)
{
	Scene* scene = new Scene();

	Model::Model* model = new Model::Model();
	Model::List* list = static_cast<Model::List*> (model->createRoot("List"));

	model->beginModification(list, "set");
	TestNodes::BinaryNode* first = list->append<TestNodes::BinaryNode>();
	TestNodes::BinaryNode* second = list->append<TestNodes::BinaryNode>();
	Model::Text* third = list->append<Model::Text>();

	first->name()->set("First node");
	TestNodes::BinaryNode* left = first->setLeft<TestNodes::BinaryNode>();
	TestNodes::BinaryNode* right = first->setRight<TestNodes::BinaryNode>();
	left->name()->set("left node");
	right->name()->set("right node");

	second->name()->set("Empty node");

	third->set("Some independent text");
	model->endModification();

	VList* l = dynamic_cast<VList*> (scene->renderer()->render(NULL, list));
	scene->addTopLevelItem(l);
	scene->scheduleUpdate();
	QApplication::processEvents();

	l->at<VExtendable>(0)->setExpanded();
	scene->scheduleUpdate();
	scene->listenToModel(model);

	// Create view
	MainView* view = new MainView(scene);
	CHECK_CONDITION(view != NULL);
}

}
