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

#include "modelbase.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "model/Model.h"
#include "nodes/List.h"
#include "nodes/Integer.h"
#include "nodes/Text.h"
#include "visitor/VisitorDefinition.h"

namespace Model {

class VisitorA : public Visitor<VisitorA, QString>{
	public:
		static void init()
		{
			addType<Node>( [](VisitorA*, Node* ) -> QString {
				return "Node";
			});

			addType<List>( [](VisitorA* v, List* n) -> QString {
				QString res = "List(";
				for(auto node : *n) res += v->visit(node) + ",";
				res+=")";
				return res;
			});

			addType<Text>( [](VisitorA*, Text*) -> QString {
				return "Text";
			});
		}
};

class VisitorB : public ExtendedVisitor<VisitorB, VisitorA>{
	public:
		static void init()
		{
			addType<Text>( [](VisitorB*, Text* t) -> QString {
				return t->get();
			});

			addType<Integer>( [](VisitorB*, Integer* i) -> QString {
				return QString::number(i->get());
			});
		}
};

class VisitorC : public Visitor<VisitorC>{
	public:
		static void init()
		{
			addType<Text>( [](VisitorC* v, Text* t) {
				v->text += t->get();
			});

			addType<Integer>( [](VisitorC* v, Integer* i) {
				v->text += QString::number(i->get());
			});
		}

		QString text;
};

TEST(ModelBase, VisitorSample)
{
	Model model;
	List* root = dynamic_cast<List*> (model.createRoot("List"));

	model.setName("root");

	model.beginModification(root, "make tree");
	root->append(new Text("hello"));
	auto i = new Integer();
	i->set(42);
	root->append( i);
	model.endModification();

	VisitorA::init();
	auto visA = new VisitorA();
	QString valA = visA->visit(root);
	delete visA;

	VisitorB::init();
	auto visB = new VisitorB();
	QString valB = visB->visit(root);
	delete visB;

	VisitorC::init();
	auto visC = new VisitorC();
	visC->visit(root);
	QString valC = visC->text;
	delete visC;

	CHECK_STR_EQUAL("List(Text,Node,)", valA);
	CHECK_STR_EQUAL("List(hello,42,)", valB);
	CHECK_STR_EQUAL("hello42", valC);
	CHECK_CONDITION( true );
}

}
