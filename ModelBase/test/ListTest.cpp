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
#include "nodes/Text.h"
#include "nodes/Integer.h"
#include "nodes/Reference.h"
#include "nodes/List.h"

namespace Model {

TEST(ModelBase, ListCreation)
{
	auto root = new List();
	Model model(root);

	CHECK_INT_EQUAL(0, root->size());

	model.beginModification(root, "add elements to list");

	Text* a = new Text();
	Text* b = new Text();
	Integer* c = new Integer();

	a->set("first");
	b->set("second");
	c->set(3);

	root->append(a);
	root->append(b);
	root->append(c);
	model.endModification();

	CHECK_INT_EQUAL(3, root->size());
	CHECK_CONDITION(root->at<Node>(0) == a);
	CHECK_CONDITION(root->at<Node>(1) == b);
	CHECK_CONDITION(root->at<Node>(2) == c);
}

TEST(ModelBase, ListInsertion)
{
	auto root = new List();
	Model model(root);

	model.beginModification(root, "add elements to list");
	Text* a = new Text();
	Text* b = new Text();
	Text* c = new Text();
	Text* d = new Text();
	Text* e = new Text();
	Text* f = new Text();
	Text* g = new Text();

	root->append(a);
	root->prepend(b);
	root->append(c);
	root->prepend(d);
	root->insert(2, e);
	root->insert(3, f);
	root->insert(1, g);
	model.endModification();

	CHECK_INT_EQUAL(7, root->size());
	CHECK_CONDITION(root->first<Node>() == d);
	CHECK_CONDITION(root->at<Node>(1) == g);
	CHECK_CONDITION(root->at<Node>(2) == b);
	CHECK_CONDITION(root->at<Node>(3) == e);
	CHECK_CONDITION(root->at<Node>(4) == f);
	CHECK_CONDITION(root->at<Node>(5) == a);
	CHECK_CONDITION(root->last<Node>() == c);
}

TEST(ModelBase, ListRemoval)
{
	auto root = new List();
	Model model(root);

	model.beginModification(root, "add elements to list");
	Text* a = new Text();
	Text* b = new Text();
	Text* c = new Text();
	Text* d = new Text();
	Text* e = new Text();
	Text* f = new Text();
	Text* g = new Text();

	root->append(a);
	root->append(b);
	root->append(c);
	root->append(d);
	root->append(e);
	root->append(f);
	root->append(g);
	model.endModification();

	model.beginModification(root, "remove elements from list");
	root->remove(3);
	root->remove(a);
	root->remove(4);
	root->remove(b);
	model.endModification();

	CHECK_CONDITION(d != nullptr);
	CHECK_CONDITION(g != nullptr);

	CHECK_INT_EQUAL(3, root->size());
	CHECK_CONDITION(root->first<Node>() == c);
	CHECK_CONDITION(root->at<Node>(1) == e);
	CHECK_CONDITION(root->last<Node>() == f);
}

TEST(ModelBase, ListUndo)
{
	auto root = new List();
	Model model(root);

	model.beginModification(root, "add elements to list");
	Text* a = new Text();
	Text* b = new Text();
	Text* c = new Text();
	Text* d = new Text();
	Text* e = new Text();
	Text* f = new Text();
	Text* g = new Text();

	root->append(a);
	root->append(b);
	root->append(c);
	root->append(d);
	root->append(e);
	root->append(f);
	root->append(g);
	model.endModification();

	CHECK_INT_EQUAL(7, root->size());
	CHECK_CONDITION(root->at<Node>(0) == a);
	CHECK_CONDITION(root->at<Node>(1) == b);
	CHECK_CONDITION(root->at<Node>(2) == c);
	CHECK_CONDITION(root->at<Node>(3) == d);
	CHECK_CONDITION(root->at<Node>(4) == e);
	CHECK_CONDITION(root->at<Node>(5) == f);
	CHECK_CONDITION(root->at<Node>(6) == g);

	model.beginModification(nullptr);
	model.undo();
	model.endModification();

	CHECK_INT_EQUAL(0, root->size());

	model.beginModification(nullptr);
	model.redo();
	model.endModification();

	CHECK_INT_EQUAL(7, root->size());
	CHECK_CONDITION(root->at<Node>(0) == a);
	CHECK_CONDITION(root->at<Node>(1) == b);
	CHECK_CONDITION(root->at<Node>(2) == c);
	CHECK_CONDITION(root->at<Node>(3) == d);
	CHECK_CONDITION(root->at<Node>(4) == e);
	CHECK_CONDITION(root->at<Node>(5) == f);
	CHECK_CONDITION(root->at<Node>(6) == g);

	model.beginModification(root, "add elements to list");
	root->remove(a);
	root->remove(e);
	model.endModification();

	CHECK_INT_EQUAL(5, root->size());
	CHECK_CONDITION(root->at<Node>(0) == b);
	CHECK_CONDITION(root->at<Node>(1) == c);
	CHECK_CONDITION(root->at<Node>(2) == d);
	CHECK_CONDITION(root->at<Node>(3) == f);
	CHECK_CONDITION(root->at<Node>(4) == g);

	model.beginModification(nullptr);
	model.undo();
	model.endModification();

	CHECK_INT_EQUAL(7, root->size());
	CHECK_CONDITION(root->at<Node>(0) == a);
	CHECK_CONDITION(root->at<Node>(1) == b);
	CHECK_CONDITION(root->at<Node>(2) == c);
	CHECK_CONDITION(root->at<Node>(3) == d);
	CHECK_CONDITION(root->at<Node>(4) == e);
	CHECK_CONDITION(root->at<Node>(5) == f);
	CHECK_CONDITION(root->at<Node>(6) == g);

	model.beginModification(nullptr);
	model.redo();
	model.endModification();

	CHECK_INT_EQUAL(5, root->size());
	CHECK_CONDITION(root->at<Node>(0) == b);
	CHECK_CONDITION(root->at<Node>(1) == c);
	CHECK_CONDITION(root->at<Node>(2) == d);
	CHECK_CONDITION(root->at<Node>(3) == f);
	CHECK_CONDITION(root->at<Node>(4) == g);

}

}
