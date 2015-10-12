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

#include "AstMapping.h"
#include "ClangHelper.h"
#include "cppimport_api.h"

#include "MacroExpansion.h"
#include "clang/Lex/MacroArgs.h"

namespace CppImport {

class DefinitionManager;
class LexicalHelper;

class CPPIMPORT_API ExpansionManager
{
	public:
		ExpansionManager(ClangHelper* clang, AstMapping* astMapping, DefinitionManager* definitionManager,
							  LexicalHelper* lexicalHelper);

		void addMacroExpansion(clang::SourceRange sr, const clang::MacroDirective* md,
															const clang::MacroArgs* args);

		QVector<MacroExpansion*> expansions();
		void clear();

		QVector<MacroExpansion*> topLevelExpansions();

		MacroExpansion* immediateExpansion(clang::SourceLocation loc);
		QSet<MacroExpansion*> expansion(Model::Node* node);

		QVector<Model::Node*> tLExpansionTLNodes(MacroExpansion* expansion);
		QVector<Model::Node*> nTLExpansionTLNodes(MacroExpansion* expansion);

	private:
		ClangHelper* clang_;
		AstMapping* astMapping_;
		DefinitionManager* definitionManager_;
		LexicalHelper* lexicalHelper_;
		MacroExpansion* currentXMacroParent {};
		QHash<Model::Node*, QSet<MacroExpansion*>> expansionCache_;
		QVector<MacroExpansion*> expansions_;

		MacroExpansion* expansion(clang::SourceLocation loc);

};

}
