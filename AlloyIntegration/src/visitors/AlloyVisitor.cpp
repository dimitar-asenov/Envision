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
        *fragment << t->name() + ":" + OOInteraction::StringComponents::stringForNode(t->typeExpression()) + ",";
        return fragment;
    });

    Visitor::addType<MethodCallExpression>( [](AlloyVisitor* v, MethodCallExpression* t) -> Export::SourceFragment*
    {
        auto fragment = new Export::CompositeFragment(t);
        if (OOInteraction::StringComponents::stringForNode(t->callee()).startsWith("Contract"))
        {
            *fragment << "fact { ";
            *fragment << v->visit(t->arguments()->at(0));
            *fragment << " }\n";
        }
        return fragment;
    });

    Visitor::addType<BinaryOperation>( [](AlloyVisitor*, BinaryOperation* t) -> Export::SourceFragment*
    {
        auto fragment = new Export::CompositeFragment(t);
        switch (t->op())
        {
            case BinaryOperation::NOT_EQUALS:
                //TODO
                break;
            default:
            break;
        }

        return fragment;
    });


}

}
