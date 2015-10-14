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

#pragma once

#include "cppimport_api.h"

#include "ClangHelper.h"
#include "NodeMapping.h"
#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

class MacroExpansion;
class MacroArgumentInfo;
class MacroArgumentLocation;
class DefinitionManager;
class ExpansionManager;
class LexicalHelper;
class XMacroManager;

class CPPIMPORT_API MetaDefinitionManager
{
	public:
		MetaDefinitionManager(OOModel::Project* root, ClangHelper* clang, DefinitionManager* definitionManager,
								ExpansionManager* expansionManager, LexicalHelper* lexicalHelper);

		OOModel::MetaDefinition* createMetaDef(const clang::MacroDirective* md);

		void createMetaDefinitionBody(OOModel::MetaDefinition* metaDef, QVector<Model::Node*> nodes,
												MacroExpansion* expansion, NodeMapping* mapping,
												QVector<MacroArgumentInfo>& arguments);

		OOModel::MetaDefinition* metaDefinition(const clang::MacroDirective* md);

		OOModel::Declaration* metaDefinitionParent(const clang::MacroDirective* md);

	private:
		OOModel::Project* root_{};
		ClangHelper* clang_{};
		DefinitionManager* definitionManager_{};
		ExpansionManager* expansionManager_{};
		LexicalHelper* lexicalHelper_{};

		QHash<QString, OOModel::MetaDefinition*> metaDefinitions_;

		/**
		 * insert all non-xMacro child meta calls into metaDef.
		 */
		void insertChildMetaCalls(MacroExpansion* expansion, NodeMapping* childMapping);

		/**
		 * return all children of node that do not belong to expansion.
		 */
		void childrenUnownedByExpansion(Model::Node* node, MacroExpansion* expansion,
																NodeMapping* mapping, QVector<Model::Node*>* result);

		/**
		 * remove all children of node that do not belong to expansion.
		 * return true if node itself does not belong to expansion.
		 */
		bool removeUnownedNodes(Model::Node* cloned, MacroExpansion* expansion,	NodeMapping* mapping);

		/**
		 * insert splices for all nodes in childMapping that are a macro argument.
		 */
		void insertArgumentSplices(NodeMapping* mapping, NodeMapping* childMapping, QVector<MacroArgumentInfo>& arguments);
};

}
