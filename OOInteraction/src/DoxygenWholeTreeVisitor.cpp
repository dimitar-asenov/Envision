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

#include "DoxygenWholeTreeVisitor.h"
#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Field.h"
#include "OOInteraction/src/string_offset_providers/StringComponents.h"

using namespace OOModel;

namespace OOInteraction {

void DoxygenWholeTreeVisitor::init()
{
	Visitor::addType<Project>( [](DoxygenWholeTreeVisitor* v, Project* t) -> QString
	{
		QString res = "";
		//res += aDoxyCommentVisitor_->visit(t);
		for (auto node : *t->classes()) res += v->visit(node);
		for (auto node : *t->modules()) res += v->visit(node);
		return res;
	});

	Visitor::addType<Module>( [](DoxygenWholeTreeVisitor* v, Module* t) -> QString
	{
		QString res = "";
		res += aDoxyCommentVisitor_->visit(t);
		res += "namespace " + t->name();
		res += "\n{\n";
		for (auto node : *t->fields()) res += v->visit(node);
		for (auto node : *t->classes()) res += v->visit(node);
		for (auto node : *t->modules()) res += v->visit(node);
		res += "}\n\n";
		return res;
	});

	Visitor::addType<Class>( [](DoxygenWholeTreeVisitor* v, Class* t) -> QString
	{
		QString res = "";
		res += aDoxyCommentVisitor_->visit(t);
		res += "public: class " + t->name();
		if (!t->baseClasses()->isEmpty()) res += " : ";
		for (auto node : *t->baseClasses()) res += StringComponents::stringForNode(node) + ",";
		if (!t->baseClasses()->isEmpty()) res.truncate(res.length()-1);
		res += "\n{\n";
		for (auto node : *t->fields()) res += v->visit(node);
		for (auto node : *t->methods()) res += v->visit(node);
		for (auto node : *t->classes()) res += v->visit(node);
		res += "};\n\n";
		return res;
	});

	Visitor::addType<Method>( [](DoxygenWholeTreeVisitor*, Method* t) -> QString
	{
		QString res = "";
		res += aDoxyCommentVisitor_->visit(t);
		res += "public: ";
		if (t->results()->isEmpty())
			res += "void ";
		else
			res += StringComponents::stringForNode(t->results()->at(0)->typeExpression()) + " ";
		res += t->name() + "(";
		for (auto node : *t->arguments())
			res += StringComponents::stringForNode(node->typeExpression()) + " " + node->name() + ",";
		if (!t->arguments()->isEmpty()) res.truncate(res.length()-1);
		res += ")";
		res += "\n{\n";
		res += "}\n\n";
		return res;
	});

	Visitor::addType<Field>( [](DoxygenWholeTreeVisitor*, Field* t) -> QString
	{
		QString res = "";
		res += aDoxyCommentVisitor_->visit(t);
		res += "public: " + StringComponents::stringForNode(t->typeExpression()) + " " + t->name() + ";\n";
		return res;
	});

}

DoxygenCommentsOnlyVisitor* DoxygenWholeTreeVisitor::aDoxyCommentVisitor_ = new DoxygenCommentsOnlyVisitor{};

}
