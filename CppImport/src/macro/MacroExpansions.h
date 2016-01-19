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

namespace Model {
	class Node;
}

namespace CppImport {

class ClangHelpers;
class MacroDefinitions;
class MacroExpansion;

/**
 * creates and stores all MacroExpansions for a translation unit.
 */
class CPPIMPORT_API MacroExpansions
{
	public:
		MacroExpansions(ClangHelpers& clang, const MacroDefinitions& macroDefinitions);

		void addMacroExpansion(clang::SourceRange sourceRange, const clang::MacroDirective* macroDirective,
									  const clang::MacroArgs* macroArguments);

		QList<MacroExpansion*> expansions() const;

		/**
		 * return all registered expansions that are not children of other expansions.
		 */
		QList<MacroExpansion*> topLevelExpansions() const;

		MacroExpansion* immediateExpansion(clang::SourceLocation loc) const;

		/**
		 * return all expansions that node is a part of.
		 */
		QSet<MacroExpansion*> expansions(Model::Node* node);

		/**
		 * used to change the origin criterium for what nodes are returned by method topLevelNodes.
		 */
		enum NodeOriginType {
			Transitive, /**< return all nodes created directly by expansion or by one of its child expansions. */
			Direct /**< return only the nodes created directly by expansion. */
		};

		/**
		 * return all top level nodes of expansion matching the origin criterium.
		 */
		QList<Model::Node*> topLevelNodes(MacroExpansion* expansion, NodeOriginType filter);

		void clear();

	private:
		ClangHelpers& clang_;
		const MacroDefinitions& macroDefinitions_;
		MacroExpansion* currentXMacroParent {};
		QHash<Model::Node*, QSet<MacroExpansion*>> expansionCache_;
		QList<MacroExpansion*> expansions_;

		/**
		 * return the top most expansion registered for loc.
		 */
		MacroExpansion* expansion(clang::SourceLocation loc) const;
};

inline QList<MacroExpansion*> MacroExpansions::expansions() const { return expansions_; }

}
