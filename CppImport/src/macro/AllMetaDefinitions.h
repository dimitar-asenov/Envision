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

#include "../cppimport_api.h"

#include "StandardMetaDefinitions.h"

namespace Model {
	class List;
}

namespace OOModel {
	class Project;
	class MetaCallExpression;
	class Class;
	class Declaration;
}

namespace CppImport {

/**
 * provides the interface to generate all sorts of MetaDefinitions during macro import.
 * processes special macro types (partial begin macro specialization, x-Macro).
 * complete syntactic macros are processed in the child component StandardMetaDefinitions.
 * creates and stores x-Macro MetaDefinitions.
 */
class CPPIMPORT_API AllMetaDefinitions
{
	public:
		AllMetaDefinitions(OOModel::Project* root, ClangHelpers& clangHelper,
								 const MacroDefinitions& macroDefinitions, MacroExpansions& macroExpansions);

		void createMetaDef(QList<Model::Node*> nodes, MacroExpansion* expansion, NodeToCloneMap& mapping,
								 QList<MacroArgumentInfo>& arguments);

		void handleXMacros();

	private:
		OOModel::Project* root_{};
		ClangHelpers& clang_;
		const MacroDefinitions& macroDefinitions_;
		MacroExpansions& macroExpansions_;
		StandardMetaDefinitions standardMetaDefinitions_;

		QHash<QString, OOModel::MetaDefinition*> xMacrometaDefinitions_;
		QHash<QString, Model::List*> specializations_;
		QSet<OOModel::MetaCallExpression*> specialized_;

		OOModel::MetaDefinition* createXMacroMetaDef(MacroExpansion* hExpansion, MacroExpansion* cppExpansion);
		void mergeClasses(OOModel::Class* merged, OOModel::Class* mergee);
		OOModel::MetaDefinition* xMacroMetaDefinition(const clang::MacroDirective* md);
		MacroExpansion* matchingXMacroExpansion(Model::Node* node);

		MacroExpansion* basePartialBegin(MacroExpansion* partialBeginExpansion);

		void handlePartialBeginSpecialization(OOModel::Declaration* metaDefParent, OOModel::MetaDefinition* metaDef,
														  MacroExpansion* expansion, MacroExpansion* beginChild);

		MacroExpansion* partialBeginChild(MacroExpansion* expansion);

		void applyPartialBeginSpecializationTransformation(MacroExpansion* hExpansion, MacroExpansion* cppExpansion);
		OOModel::Declaration* metaDefinitionParent(const clang::MacroDirective* md);
};

}
