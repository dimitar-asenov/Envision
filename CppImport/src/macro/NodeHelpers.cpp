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

#include "NodeHelpers.h"

#include "NodeToCloneMap.h"

#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

void NodeHelpers::orderNodes(QVector<Model::Node*>& input)
{
	qSort(input.begin(), input.end(),
			[](Model::Node* e1, Model::Node* e2)
	{
		if (auto commonAncestor = e1->lowestCommonAncestor(e2))
			if (auto list = DCast<Model::List>(commonAncestor))
			{
				int index1 = -1;
				for (auto c : list->children())
					if (c == e1 || c->isAncestorOf(e1))
					{
						index1 = list->indexOf(c);
						break;
					}

				int index2 = -1;
				for (auto c : list->children())
					if (c == e2 || c->isAncestorOf(e2))
					{
						index2 = list->indexOf(c);
						break;
					}

				return index1 < index2;
			}

		return true;
	});
}

bool NodeHelpers::validContext(Model::Node* node)
{
	if (DCast<OOModel::Project>(node))
		return true;
	else if (DCast<OOModel::Module>(node))
		return true;
	else if (DCast<OOModel::Class>(node))
		return true;
	else if (DCast<OOModel::Method>(node))
		return true;
	else
		return false;
}

OOModel::Declaration* NodeHelpers::actualContext(Model::Node* node)
{
	auto current = node->parent();

	while (current)
	{
		if (auto result = DCast<OOModel::Project>(current))
			return result;
		else if (auto result = DCast<OOModel::Module>(current))
			return result;
		else if (auto result = DCast<OOModel::Class>(current))
			return result;
		else if (auto result = DCast<OOModel::Method>(current))
			return result;
		else
			current = current->parent();
	}

	Q_ASSERT(false);
}

OOModel::Declaration* NodeHelpers::createContext(OOModel::Declaration* actualContext)
{
	if (DCast<OOModel::Project>(actualContext))
		return new OOModel::Project("Context");
	else if (DCast<OOModel::Module>(actualContext))
		return new OOModel::Module("Context");
	else if (DCast<OOModel::Class>(actualContext))
		return new OOModel::Class("Context");
	else if (DCast<OOModel::Method>(actualContext))
		return new OOModel::Method("Context");

	Q_ASSERT(false);
}

Model::Node* NodeHelpers::cloneWithMapping(Model::Node* node, NodeToCloneMap& mapping)
{
	auto clone = node->clone();

	QList<Model::Node*> info;
	buildMappingInfo(node, info);
	useMappingInfo(clone, info, mapping);

	return clone;
}

void NodeHelpers::removeNode(Model::Node* node, bool removeMetaCalls)
{
	if (!node || !node->parent()) return;

	if (!removeMetaCalls)
		while (auto metaCall = containsMetaCall(node))
		{
			return;

			if (auto declaration = DCast<OOModel::Declaration>(metaCall->parent()->parent()))
			{
				auto newDeclaration = node->firstAncestorOfType<OOModel::Declaration>();

				declaration->metaCalls()->remove(declaration->metaCalls()->indexOf(metaCall));
				newDeclaration->metaCalls()->append(metaCall);
			}
		}

	if (auto ooList = DCast<Model::List>(node->parent()))
		ooList->remove(ooList->indexOf(node));
	else if (auto ooVarDecl = DCast<OOModel::VariableDeclaration>(node->parent()))
	{
		if (ooVarDecl->initialValue() == node)
			ooVarDecl->setInitialValue(nullptr);
	}
	else if (auto skip = DCast<OOModel::VariableDeclarationExpression>(node->parent()))
		removeNode(skip, removeMetaCalls);
	else if (auto skip = DCast<OOModel::ExpressionStatement>(node->parent()))
		removeNode(skip, removeMetaCalls);
	else
		qDebug() << "not removed" << node->typeName() << "in" << node->parent()->typeName();
}

void NodeHelpers::addNodeToDeclaration(Model::Node* node, OOModel::Declaration* declaration)
{
	if (auto ooExpression = DCast<OOModel::Expression>(node))
	{
		if (auto context = DCast<OOModel::Method>(declaration))
			context->items()->append(new OOModel::ExpressionStatement(ooExpression));
		else
			qDebug() << "can not insert expression" << ooExpression->typeName() << "into a" << declaration->typeName();
	}
	else if (auto ooStatement = DCast<OOModel::Statement>(node))
	{
		if (auto context = DCast<OOModel::Method>(declaration))
			context->items()->append(ooStatement);
		else
			Q_ASSERT(false);
	}
	else if (auto ooDeclaration = DCast<OOModel::Declaration>(node))
	{
		if (auto ooClass = DCast<OOModel::Class>(node))
		{
			if (auto context = DCast<OOModel::Project>(declaration))
				context->classes()->append(ooClass);
			else if (auto context = DCast<OOModel::Module>(declaration))
				context->classes()->append(ooClass);
			else if (auto context = DCast<OOModel::Class>(declaration))
				context->classes()->append(ooClass);
			else
				Q_ASSERT(false);
		}
		else if (auto ooField = DCast<OOModel::Field>(node))
		{
			if (auto context = DCast<OOModel::Project>(declaration))
				context->fields()->append(ooField);
			else if (auto context = DCast<OOModel::Module>(declaration))
				context->fields()->append(ooField);
			else if (auto context = DCast<OOModel::Class>(declaration))
				context->fields()->append(ooField);
			else
				Q_ASSERT(false);
		}
		else if (auto ooVarDecl = DCast<OOModel::VariableDeclaration>(node))
		{
			if (auto context = DCast<OOModel::Method>(declaration))
				context->items()->append(new OOModel::ExpressionStatement(
													 new OOModel::VariableDeclarationExpression(ooVarDecl)));
			else
				Q_ASSERT(false);
		}
		else if (auto ooMethod = DCast<OOModel::Method>(node))
		{
			if (auto context = DCast<OOModel::Project>(declaration))
				context->methods()->append(ooMethod);
			else if (auto context = DCast<OOModel::Module>(declaration))
				context->methods()->append(ooMethod);
			else if (auto context = DCast<OOModel::Class>(declaration))
				context->methods()->append(ooMethod);
			else
				Q_ASSERT(false);
		}
		else
		{
			if (auto context = DCast<OOModel::Project>(declaration))
				context->subDeclarations()->append(ooDeclaration);
			else if (auto context = DCast<OOModel::Module>(declaration))
				context->subDeclarations()->append(ooDeclaration);
			else if (auto context = DCast<OOModel::Class>(declaration))
				context->subDeclarations()->append(ooDeclaration);
			else if (auto context = DCast<OOModel::Method>(declaration))
				context->subDeclarations()->append(ooDeclaration);
			else
				Q_ASSERT(false);
		}
	}
	else
		Q_ASSERT(false && "not implemented");
}

QVector<Model::Node*> NodeHelpers::topLevelNodes(QVector<Model::Node*> input)
{
	QSet<Model::Node*> topLevel;
	for (auto node : input) topLevel.insert(node);

	for (auto node : input)
		for (auto other : input)
			if (node != other)
				if (node->isAncestorOf(other))
					topLevel.remove(other);

	QVector<Model::Node*> result;
	for (auto node : topLevel) result.append(node);
	return result;
}

void NodeHelpers::buildMappingInfo(Model::Node* node, QList<Model::Node*>& info)
{
	info.push_back(node);

	for (auto child : node->children())
		buildMappingInfo(child, info);
}

void NodeHelpers::useMappingInfo(Model::Node* node, QList<Model::Node*>& info, NodeToCloneMap& mapping)
{
	mapping.add(info.front(), node);
	info.pop_front();

	for (auto child : node->children())
		useMappingInfo(child, info, mapping);
}

OOModel::MetaCallExpression* NodeHelpers::containsMetaCall(Model::Node* node)
{
	if (auto metaCall = DCast<OOModel::MetaCallExpression>(node))
		return metaCall;

	for (auto child : node->children())
		if (auto metaCall = containsMetaCall(child))
			return metaCall;

	return nullptr;
}

bool NodeHelpers::stringMatches(const QString& regex, const QString& value)
{
	QRegularExpression regEx(regex);
	return regEx.match(value).hasMatch();
}

OOModel::Declaration* NodeHelpers::findDeclaration(Model::List* list, const QString& name)
{
	for (auto i = 0; i < list->size(); i++)
		if (auto decl = DCast<OOModel::Declaration>(list->at(i)))
			if (decl->name() == name)
				return decl;

	return nullptr;
}

OOModel::Expression* NodeHelpers::createNameExpressionFromString(const QString& input)
{
	QString baseCase = "((::)?(\\w+(::|\\.|->))*\\w+(\\*|&)?)";

	if (stringMatches("^" + baseCase + "$", input))
	{
		QRegularExpression regEx("(::|\\.)");
		QStringList parts = input.split(regEx);

		QString basePart = parts.last();

		OOModel::Expression* result = nullptr;
		OOModel::ReferenceExpression* current = nullptr;

		if (basePart.endsWith("&"))
		{
			current = new OOModel::ReferenceExpression(basePart.left(basePart.length() - 1));
			result = new OOModel::ReferenceTypeExpression(current);
		}
		else if (basePart.endsWith("*"))
		{
			current = new OOModel::ReferenceExpression(basePart.left(basePart.length() - 1));
			result = new OOModel::PointerTypeExpression(current);
		}
		else
		{
			current = new OOModel::ReferenceExpression(basePart);
			result = current;
		}

		for (auto i = parts.size() - 2; i >= 0; i--)
		{
			QString part = parts[i];

			if (part.isEmpty())
			{
				Q_ASSERT(i == 0);
				current->setPrefix(new OOModel::GlobalScopeExpression());
			}
			else
			{
				auto next = new OOModel::ReferenceExpression(part);
				current->setPrefix(next);
				current = next;
			}
		}

		return result;
	}
	else if (stringMatches("^" + baseCase + "<" + baseCase + ">$", input))
	{
		QStringList split = input.split("<");
		auto baseRef = DCast<OOModel::ReferenceExpression>(createNameExpressionFromString(split[0]));
		Q_ASSERT(baseRef);
		auto typeArg = createNameExpressionFromString(split[1].left(split[1].length() - 1));
		baseRef->typeArguments()->append(typeArg);
		return baseRef;
	}
	else
	{
		qDebug() << "createReferenceExpressionFromString failed on input:" << input;
		return new OOModel::ReferenceExpression(input);
	}
}

}
