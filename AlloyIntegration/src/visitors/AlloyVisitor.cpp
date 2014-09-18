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

#include "AlloyVisitor.h"
#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Field.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/expressions/BinaryOperation.h"
#include "OOModel/src/expressions/UnaryOperation.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/expressions/ThisExpression.h"
#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/elements/OOReference.h"
#include "OOModel/src/expressions/IntegerLiteral.h"

#include "OOInteraction/src/string_offset_providers/StringComponents.h"

using namespace OOModel;

namespace Alloy {

void AlloyVisitor::init()
{
	Visitor::addType<Project>( [](AlloyVisitor* v, Project* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		for (auto node : *t->classes()) *fragment << v->visit(node);
		for (auto node : *t->modules()) *fragment << v->visit(node);
		return fragment;
	});

	Visitor::addType<Module>( [](AlloyVisitor* v, Module* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		//for (auto node : *t->fields()) *fragment << v->visit(node);
		for (auto node : *t->classes()) *fragment << v->visit(node);
		for (auto node : *t->modules()) *fragment << v->visit(node);
		return fragment;
	});

	Visitor::addType<Class>( [](AlloyVisitor* v, Class* t) -> Export::SourceFragment*
	{
		currentClass_ = t->name();
		auto fragment = new Export::CompositeFragment(t);
		*fragment << "sig " + t->name() + "";
		*fragment << "{";
		for (auto node : *t->fields()) *fragment << v->visit(node);
		*fragment << "}\n";
		for (auto node : *t->methods()) *fragment << v->visit(node);
		for (auto node : *t->classes()) *fragment << v->visit(node);
		return fragment;
	});

	Visitor::addType<Method>( [](AlloyVisitor* v, Method* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		for (auto node : *t->items()) *fragment << v->visit(node);
		return fragment;
	});

	Visitor::addType<Field>( [](AlloyVisitor*, Field* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		if (!DCast<PrimitiveTypeExpression>(t->typeExpression()))
			*fragment << t->name() + ": lone " + OOInteraction::StringComponents::stringForNode(t->typeExpression()) + ",";
		else
		{
			QString temp = OOInteraction::StringComponents::stringForNode(t->typeExpression());
			temp[0] = temp[0].toUpper();
			*fragment << t->name() + ": one " + temp + ",";
		}
		return fragment;
	});

	Visitor::addType<MethodCallExpression>( [](AlloyVisitor* v, MethodCallExpression* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		if (OOInteraction::StringComponents::stringForNode(t->callee()).startsWith("Contract.Invariant"))
		{
			*fragment << "fact {";
			*fragment << "all n:" + currentClass_ + " | ";
			*fragment << v->visit(t->arguments()->at(0));
			*fragment << "}\n";
		}
		else if (OOInteraction::StringComponents::stringForNode(t->callee()).startsWith("Contract.ForAll"))
		{
			//*fragment << v->visit(t->arguments()->at(0));
			//*fragment << v->visit(t->arguments()->at(1));
		}
		return fragment;
	});

	Visitor::addType<BinaryOperation>( [](AlloyVisitor* v, BinaryOperation* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		switch (t->op())
		{
			case BinaryOperation::NOT_EQUALS:
				*fragment << v->visit(t->left());
				*fragment << " != ";
				*fragment << v->visit(t->right());
				break;
			case BinaryOperation::EQUALS:
				*fragment << v->visit(t->left());
				*fragment << " = ";
				*fragment << v->visit(t->right());
				break;
			case BinaryOperation::GREATER:
				*fragment << v->visit(t->left());
				*fragment << " > ";
				*fragment << v->visit(t->right());
				break;
			case BinaryOperation::LESS:
				*fragment << v->visit(t->left());
				*fragment << " < ";
				*fragment << v->visit(t->right());
				break;
			case BinaryOperation::PLUS:
				*fragment << v->visit(t->left());
				*fragment << ".add[";
				*fragment << v->visit(t->right());
				*fragment << "]";
				break;
			case BinaryOperation::MINUS:
				*fragment << v->visit(t->left());
				*fragment << ".sub[";
				*fragment << v->visit(t->right());
				*fragment << "]";
				break;
			default: *fragment << "unknown"; break;
		}
		return fragment;
	});

	Visitor::addType<UnaryOperation>( [](AlloyVisitor* v, UnaryOperation* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		switch (t->op())
		{
			case UnaryOperation::PLUS: *fragment << "+"; break;
			case UnaryOperation::MINUS: *fragment << "-"; break;
			default: *fragment << "unknown"; break;
		}
		*fragment << v->visit(t->operand());
		return fragment;
	});

	Visitor::addType<ThisExpression>( [](AlloyVisitor*, ThisExpression* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		*fragment << "n";
		return fragment;
	});

	Visitor::addType<ReferenceExpression>( [](AlloyVisitor* v, ReferenceExpression* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		*fragment << v->visit(t->prefix());
		if (t->ref() != nullptr)
		{
			*fragment << ".";
			*fragment << v->visit(t->ref());
		}
		return fragment;
	});

	Visitor::addType<OOReference>( [](AlloyVisitor*, OOReference* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		*fragment << t->name();
		return fragment;
	});

	Visitor::addType<IntegerLiteral>( [](AlloyVisitor*, IntegerLiteral* t) -> Export::SourceFragment*
	{
		auto fragment = new Export::CompositeFragment(t);
		*fragment << QString::number(t->value());
		return fragment;
	});

}

QString AlloyVisitor::currentClass_ = "";

}
