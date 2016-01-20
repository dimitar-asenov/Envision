/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "CodeGenerationVisitor.h"

#include "declarations/MetaDefinition.h"
#include "expressions/BooleanLiteral.h"
#include "expressions/StringLiteral.h"
#include "expressions/ReferenceExpression.h"
#include "expressions/MetaCallExpression.h"

#include "ModelBase/src/nodes/NameText.h"
#include "OOModel/src/elements/FormalResult.h"

namespace OOModel {

CodeGenerationVisitor::CodeGenerationVisitor(QMap<QString, Model::Node *> args) : args_{args} {}

void CodeGenerationVisitor::init()
{
	addType<ReferenceExpression>(visitReferenceExpression);
	addType<Model::NameText>(visitNameText);
	addType<MetaCallExpression>(visitMetaCallExpression);
}

void CodeGenerationVisitor::visitReferenceExpression(CodeGenerationVisitor* v, OOModel::ReferenceExpression* n)
{
	auto input = n->name();

	if (!input.contains("##"))
	{
		if (input.startsWith("#"))
		{
			if (auto argument = v->args_[input.right(input.length() - 1)])
				if (auto argumentReference = DCast<ReferenceExpression>(argument))
					n->parent()->replaceChild(n, new OOModel::StringLiteral{argumentReference->name()});
		}
		else
			if (auto argument = v->args_[input])
			{
				if (auto argumentReference = DCast<ReferenceExpression>(argument))
				{
					// case: there exists a replacement and it is another ReferenceExpression
					// -> copy name (replacing it would cause child nodes of n to disappear)

					n->setName(argumentReference->name());
				}
				else
				{
					// case: there exists a replacement and it is not a ReferenceExpression
					// -> replace node

					Model::Node* cloned = nullptr;
					if (DCast<OOModel::Expression>(n) && DCast<OOModel::FormalResult>(argument))
					{
						// n is an expression but the argument is a formal result so we have to use
						// the expression inside the formal result
						cloned = DCast<OOModel::FormalResult>(argument)->typeExpression()->clone();
					}
					else // default case: use whole argument
						cloned = argument->clone();
					n->parent()->replaceChild(n, cloned);

					// visit the cloned tree and return to avoid visiting the children of n
					v->visitChildren(cloned);
					return;
				}
			}
	}
	else
	{
		// case: n's name is a concatenated identifier (a##b)
		// -> build identifier

		QStringList parts = input.split("##");
		bool modified = false;

		for (auto i = 0; i < parts.size(); i++)
			if (auto argument = DCast<ReferenceExpression>(v->args_[parts[i]]))
			{
				parts[i] = argument->name();
				modified = true;
			}

		if (modified) n->setName(parts.join(""));
	}

	v->visitChildren(n);
}

void CodeGenerationVisitor::visitNameText(CodeGenerationVisitor* v, Model::NameText* n)
{
	auto input = n->get();

	if (!input.contains("##"))
	{
		if (auto argument = DCast<ReferenceExpression>(v->args_[input]))
		{
			// case: n's name is an existing argument of type ReferenceExpression
			// -> copy name as new text

			n->set(argument->name());
		}
	}
	else
	{
		// case: n's text is a concatenated text (a##b)
		// -> build text

		QStringList parts = input.split("##");
		bool modified = false;

		for (auto i = 0; i < parts.size(); i++)
			if (auto argument = DCast<ReferenceExpression>(v->args_[parts[i]]))
			{
				parts[i] = argument->name();
				modified = true;
			}

		if (modified) n->set(parts.join(""));
	}

	v->visitChildren(n);
}

void CodeGenerationVisitor::visitMetaCallExpression(CodeGenerationVisitor* v, MetaCallExpression* n)
{
	/*
	 * process arguments before generating.
	 * this ensures proper argument propagation.
	 */
	v->visitChildren(n);

	if (!n->metaDefinition())
	{
		// case: unresolved meta definition
		// -> check if it is a predefined meta function

		if (auto metaDef = DCast<ReferenceExpression>(n->callee()))
			v->handlePredefinedFunction(metaDef->name(), n);
	}
	else
	{
		// case: resolved meta definition
		// -> generate recursively

		n->generatedTree();
	}
}

void CodeGenerationVisitor::handlePredefinedFunction(QString function, MetaCallExpression* n)
{
	/*
	 * only handle predefined functions if they are nested in another meta call.
	 * this helps preventing unintentional modification of parents which are not generated nodes.
	 */
	if (!n->firstAncestorOfType<MetaCallExpression>()) return;

	if (function == "SET_OVERRIDE_FLAG")
	{
		if (n->arguments()->size() != 1)
		{
			qDebug() << function << "#arguments != 1";
			return;
		}

		if (auto argument = DCast<ReferenceExpression>(n->arguments()->first()))
			if (auto flag = DCast<BooleanLiteral>(args_[argument->name()]))
				if (auto p = n->firstAncestorOfType<Declaration>())
					p->modifiers()->set(Modifier::Override, flag->value());
	}
}

}
