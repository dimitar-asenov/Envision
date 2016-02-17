/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#pragma once

#include "cppexport_api.h"

namespace Model
{
	class Node;
}

namespace OOModel
{
	class Declaration;
	class Method;
	class Class;
	class Module;
}

namespace Export
{
	class CompositeFragment;
}

namespace CppExport {

class CodeComposite;
class CppPrintContext;

class CPPEXPORT_API ExportHelpers
{
	public:
		/**
		 * used to prevent exporting of declarations already provided in a meta call of the parent.
		 */
		static bool shouldExportMethod(OOModel::Method* method, bool isHeaderVisitor, bool isSourceVisitor);
		static bool methodSignaturesMatch(OOModel::Method* method, OOModel::Method* other);

		static QString pluginName(Model::Node* node);
		static QString exportFlag(Model::Node* node);
		static CodeComposite* apiInclude(Model::Node* node);

		static bool isSignalingDeclaration(OOModel::Declaration* declaration);

		template <typename T>
		static QList<T*> topologicalSort(QHash<T*, QSet<T*>> dependencies,
													std::function<T*(QList<T*>&, T*)> selector = nullptr);

		static void printDeclarationQualifier(Export::CompositeFragment* fragment, OOModel::Declaration* from,
														  Model::Node* to, bool printTypename = false);
		static OOModel::Declaration* firstValidAncestorPrintContext(Model::Node* node);

		static OOModel::Module* parentNamespaceModule(Model::Node* node);

		static bool isInlineNonPrivateOrNonTemplateClassMethod(OOModel::Method* method, CppPrintContext& printContext);
};

template <typename T>
QList<T*> ExportHelpers::topologicalSort(QHash<T*, QSet<T*>> dependsOn, std::function<T*(QList<T*>&, T*)> selector)
{
	// calculate a list of elements with no dependencies.
	// calculate a map that maps from an element to all elements that depend on it.
	QList<T*> noPendingDependencies;
	QHash<T*, QSet<T*>> neededFor;
	for (auto it = dependsOn.begin(); it != dependsOn.end(); it++)
		if (it.value().empty())
			// this element depends on no other elements
			noPendingDependencies.append(it.key());
		else
		{
			it.value() = it.value().intersect(dependsOn.keys().toSet());

			// for every other element this element depends on add it to the neededFor map for said other element
			bool notNeededForAnything = true;
			for (auto dependency : it.value())
				if (dependsOn.contains(dependency))
				{
					neededFor[dependency].insert(it.key());
					notNeededForAnything = false;
				}
			if (notNeededForAnything) noPendingDependencies.append(it.key());
		}
	QList<T*> result;
	while (!noPendingDependencies.empty())
	{
		// take any item form the list of item with no more dependencies and add it to the result
		auto n = selector ? selector(noPendingDependencies, result.empty() ? nullptr : result.first())
								: noPendingDependencies.takeFirst();
		result.append(n);

		// check if we are neededFor another node
		auto it = neededFor.find(n);
		if (it == neededFor.end()) continue;

		// for every node we are neededFor
		for (auto m : *it)
		{
			// find the nodes the node we are needed for dependsOn
			auto dIt = dependsOn.find(m);
			Q_ASSERT(dIt != dependsOn.end());

			// remove us from its dependencies
			dIt->remove(n);

			// if this node has no more dependencies add it to the list of items with no more dependencies

			bool noPendingDependency = true;
			for (auto d : *dIt)
				if (dependsOn.contains(d))
				{
					noPendingDependency = false;
					break;
				}
			if (noPendingDependency) noPendingDependencies.append(m);
		}
	}

	// test graph for cycles
	for (auto dependencies : dependsOn.values())
		Q_ASSERT(dependencies.empty());

	return result;
}

}
