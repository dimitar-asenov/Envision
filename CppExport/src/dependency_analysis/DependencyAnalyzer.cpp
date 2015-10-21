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

#include "DependencyAnalyzer.h"

#include "File.h"

#include "OOModel/src/allOOModelNodes.h"

namespace CppExport {

void DependencyAnalyzer::associateNodeWithFile(Model::Node* node, QString name)
{
	// insert custom tree to file adjustments here
	if (name == "Model/LoadedNode") name = "Model/PersistentStore";

	auto it = nameToFileMap_.find(name);
	if (it != nameToFileMap_.end())
	{
		(*it)->addNode(node);
		return;
	}

	auto file = new File(name, node, new File(name, node));
	nameToFileMap_.insert(name, file);
	nodeToFileMap_.insert(node, file);
}

void DependencyAnalyzer::buildNodeToFileMap(Model::Node* name, QString namespaceName)
{
	if (auto ooModule = DCast<OOModel::Module>(name))
	{
		if (ooModule->classes()->size() > 0)
			namespaceName = ooModule->name();
		else
		{
			// macro file
			associateNodeWithFile(ooModule, namespaceName + "/" + ooModule->name());
			return;
		}
	}
	else if (auto ooClass = DCast<OOModel::Class>(name))
	{
		associateNodeWithFile(ooClass, namespaceName + "/" + ooClass->name());
		return;
	}

	for (auto child : name->children())
		buildNodeToFileMap(child, namespaceName);
}

QList<File*> DependencyAnalyzer::files(Model::Node* node)
{
	buildNodeToFileMap(node->root(), "");

	auto headerFiles = nodeToFileMap_.values();

	for (File* file : headerFiles)
	{
		calculateDependencies(file);
		calculateDependencies(file->source());
	}

	return headerFiles;
}

File* DependencyAnalyzer::headerFileForNode(Model::Node* node)
{
	if (!node) return nullptr;

	auto it = nodeToFileMap_.find(node);
	return it != nodeToFileMap_.end() ? *it : headerFileForNode(node->parent());
}

void DependencyAnalyzer::calculateDependencies(File* file)
{
	QList<OOModel::ReferenceExpression*> allReferenceExpressions;
	for (auto node : file->nodes())
		allChildrenOfTypeReferenceExpression(node, allReferenceExpressions);

	for (auto referenceExpression : allReferenceExpressions)
	{
		if (file->isHeaderFile())
		{
			// do not proceed if referenceExpression is inside a method body
			if (auto ooMethod = referenceExpression->firstAncestorOfType<OOModel::Method>())
				if (ooMethod->items()->isAncestorOf(referenceExpression))
					continue;
		}

		// friends should not introduce a dependency
		if (auto ooClass = referenceExpression->firstAncestorOfType<OOModel::Class>())
			if (ooClass->friends()->isAncestorOf(referenceExpression))
				continue;

		if (auto target = resolveReference(referenceExpression))
		{
			auto targetFile = headerFileForNode(target);

			if (targetFile && targetFile != file)
				file->dependencies().addDependency(targetFile, isSoftDependency(referenceExpression));
		}
	}
}

Model::Node* DependencyAnalyzer::resolveReference(OOModel::ReferenceExpression* referenceExpression)
{
	if (auto target = referenceExpression->target()) return target;

	// insert custom resolution adjustments here

	return nullptr;
}

bool DependencyAnalyzer::isSoftDependency(OOModel::ReferenceExpression* reference)
{
	auto parent = reference->parent();
	Q_ASSERT(parent);

	if (reference->firstAncestorOfType<OOModel::MethodCallExpression>()) return false;

	if (DCast<OOModel::TypeQualifierExpression>(parent)) parent = parent->parent();
	if (!DCast<OOModel::PointerTypeExpression>(parent) && !DCast<OOModel::ReferenceTypeExpression>(parent)) return false;

	return true;
}

void DependencyAnalyzer::allChildrenOfTypeReferenceExpression(Model::Node* node,
																				  QList<OOModel::ReferenceExpression*>& result)
{
	if (auto referenceExpression = DCast<OOModel::ReferenceExpression>(node))
		result.append(referenceExpression);

	for (auto child : node->children())
		allChildrenOfTypeReferenceExpression(child, result);
}

}
