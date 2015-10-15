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
#include "EnvisionToClangMap.h"
#include "MacroExpansion.h"
#include "NodeToCloneMap.h"
#include "MacroArgumentLocation.h"
#include "MacroArgumentInfo.h"
#include "MacroExpansions.h"
#include "MacroDefinitions.h"
#include "LexicalTransformations.h"
#include "AllMetaDefinitions.h"
#include "StandardMetaDefinitions.h"
#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

class CPPIMPORT_API MacroImporter
{
	public:
		MacroImporter(OOModel::Project* root);

		void startTranslationUnit(const clang::SourceManager* sourceManager,
										  const clang::Preprocessor* preprocessor);

		/**
		 * invoked after every imported translation unit to perform macro import.
		 */
		void endTranslationUnit();

		/**
		 * insert top level meta calls and remove nodes generated from top level expansions.
		 * invoked after all translation units have been processed.
		 */
		void endEntireImport();

		void mapAst(clang::Stmt* clangAstNode, Model::Node* envisionAstNode);
		void mapAst(clang::Decl* clangAstNode, Model::Node* envisionAstNode);

	private:
		OOModel::Project* root_{};

		ClangHelpers clang_;
		EnvisionToClangMap envisionToClangMap_;
		MacroDefinitions macroDefinitions_;
		MacroExpansions macroExpansions_;
		LexicalTransformations lexicalTransformations_;
		AllMetaDefinitions allMetaDefinitions_;
		QHash<QString, OOModel::MetaCallExpression*> metaCalls_;
		QVector<Model::Node*> finalizationNodes;
		QHash<Model::Node*, MacroExpansion*> finalizationMetaCalls;

		void handleMacroExpansion(QVector<Model::Node*> nodes, MacroExpansion* expansion, NodeToCloneMap* mapping,
										  QVector<MacroArgumentInfo>& arguments);

		bool insertMetaCall(MacroExpansion* expansion);

		/**
		 * find best actual context matching expansion without nodes.
		 */
		OOModel::Declaration* actualContext(MacroExpansion* expansion);

		QVector<MacroArgumentLocation> argumentHistory(clang::SourceRange range);
		QVector<MacroArgumentLocation> argumentHistory(Model::Node* node);

		/**
		 * return all arguments which are associated to children of node.
		 */
		void allArguments(Model::Node* node, QVector<MacroArgumentInfo>* result, NodeToCloneMap* mapping);

		/**
		 * insert gathered argument nodes at their original (logical) location.
		 */
		void insertArguments(QVector<MacroArgumentInfo>& allArguments);

		/**
		 * clear all information for the current translation unit.
		 */
		void clear();

		/**
		 * calculate nodes to be removed from the tree after importing.
		 */
		void calculateFinalizationNodes(QVector<Model::Node*>& nodes, NodeToCloneMap& mapping);
};

}
