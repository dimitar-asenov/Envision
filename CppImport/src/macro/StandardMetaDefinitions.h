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

#include "ClangHelpers.h"
#include "NodeToCloneMap.h"
#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

class MacroExpansion;
class MacroArgumentInfo;
class MacroArgumentLocation;
class MacroDefinitions;
class MacroExpansions;
class LexicalTransformations;
class AllMetaDefinitions;

/**
 * creates and stores all MetaDefinitions that are not x-Macro MetaDefinitions.
 * processes all syntactically complete macro definitions.
 */
class CPPIMPORT_API StandardMetaDefinitions
{
	public:
		StandardMetaDefinitions(const ClangHelpers& clang, const MacroDefinitions& definitionManager,
									 MacroExpansions& MacroExpansions, const LexicalTransformations& lexicalHelper);

		OOModel::MetaDefinition* createMetaDef(const clang::MacroDirective* md);

		void createMetaDefinitionBody(OOModel::MetaDefinition* metaDef, QVector<Model::Node*> nodes,
												MacroExpansion* expansion, NodeToCloneMap* mapping,
												QVector<MacroArgumentInfo>& arguments);

		OOModel::MetaDefinition* metaDefinition(const clang::MacroDirective* md);

	private:
		const ClangHelpers& clang_;
		const MacroDefinitions& definitionManager_;
		MacroExpansions& macroExpansions_;
		const LexicalTransformations& lexicalTransformations_;

		QHash<QString, OOModel::MetaDefinition*> standardMetaDefinitions_;

		/**
		 * insert all non-xMacro child meta calls into metaDef.
		 */
		void insertChildMetaCalls(MacroExpansion* expansion, NodeToCloneMap* childMapping);

		/**
		 * return all children of node that do not belong to expansion.
		 */
		void childrenUnownedByExpansion(Model::Node* node, MacroExpansion* expansion, NodeToCloneMap* mapping,
												  QVector<Model::Node*>* result);

		/**
		 * remove all children of node that do not belong to expansion.
		 * return true if node itself does not belong to expansion.
		 */
		bool removeUnownedNodes(Model::Node* cloned, MacroExpansion* expansion,	NodeToCloneMap* mapping);

		/**
		 * insert splices for all nodes in childMapping that are a macro argument.
		 */
		void insertArgumentSplices(NodeToCloneMap* mapping, NodeToCloneMap* childMapping,
											QVector<MacroArgumentInfo>& arguments);

		void applyLexicalTransformations(Model::Node* node, NodeToCloneMap* mapping, QVector<QString> formalArgs) const;

		void replaceWithReference(Model::Node* current, const QString& replacement, NodeToCloneMap* mapping) const;
};

}
