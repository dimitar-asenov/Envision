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

bool NodeHelpers::validContext(Model::Node* node)
{
	return DCast<OOModel::Project>(node) || DCast<OOModel::Module>(node) || DCast<OOModel::Class>(node) ||
			 DCast<OOModel::Method>(node);
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
		return new OOModel::Project{"Context"};
	else if (DCast<OOModel::Module>(actualContext))
		return new OOModel::Module{"Context"};
	else if (DCast<OOModel::Class>(actualContext))
		return new OOModel::Class{"Context"};
	else if (DCast<OOModel::Method>(actualContext))
		return new OOModel::Method{"Context"};

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

void NodeHelpers::removeNodeFromParent(Model::Node* node, bool removeMetaCalls)
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
		removeNodeFromParent(skip, removeMetaCalls);
	else if (auto skip = DCast<OOModel::ExpressionStatement>(node->parent()))
		removeNodeFromParent(skip, removeMetaCalls);
	else
		qDebug() << "not removed" << node->typeName() << "in" << node->parent()->typeName();
}

void NodeHelpers::addNodeToDeclaration(Model::Node* node, OOModel::Declaration* declaration)
{
	if (auto ooExpression = DCast<OOModel::Expression>(node))
	{
		if (auto context = DCast<OOModel::Method>(declaration))
			context->items()->append(new OOModel::ExpressionStatement{ooExpression});
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
				context->items()->append(new OOModel::ExpressionStatement{
													 new OOModel::VariableDeclarationExpression{ooVarDecl}});
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
	else if (auto ooFormalResult = DCast<OOModel::FormalResult>(node))
	{
		if (auto context = DCast<OOModel::Method>(declaration))
		{
			context->results()->clear();
			context->results()->append(ooFormalResult);
		}
		else
			Q_ASSERT(false);
	}
	else
		Q_ASSERT(false && "not implemented");
}

QList<Model::Node*> NodeHelpers::topLevelNodes(QList<Model::Node*> input)
{
	auto end = std::remove_if(input.begin(), input.end(),
										[=] (auto e)
										{
											for (auto other : input)
												if (other != e && other->isAncestorOf(e))
													return true;

											return false;
										});

	input.erase(end, input.end());
	return input;
}

void NodeHelpers::buildMappingInfo(Model::Node* node, QList<Model::Node*>& info)
{
	info.push_back(node);

	for (auto child : node->children())
		buildMappingInfo(child, info);
}

void NodeHelpers::useMappingInfo(Model::Node* node, QList<Model::Node*>& info, NodeToCloneMap& mapping)
{
	mapping.add(info.takeFirst(), node);

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

OOModel::Declaration* NodeHelpers::findDeclaration(Model::List* list, const QString& name)
{
	for (auto elem : *list)
		if (auto decl = DCast<OOModel::Declaration>(elem))
			if (decl->name() == name)
				return decl;

	return nullptr;
}

void CppImport::NodeHelpers::removeNodesFromParent(QList<Model::Node*> nodes)
{
	for (auto n : nodes)
		removeNodeFromParent(n);
}

}
