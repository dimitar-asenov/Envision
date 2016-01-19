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

namespace OOModel {
	class MetaCallExpression;
}

namespace CppImport {

class CPPIMPORT_API MacroExpansion
{
	public:
		MacroExpansion(clang::SourceRange range, const clang::MacroDirective* definition, MacroExpansion* parent,
							MacroExpansion* xMacroParent);

		bool isChildOf(MacroExpansion* entry);

		clang::SourceRange range();
		const clang::MacroDirective* definition();
		MacroExpansion* parent();
		QList<clang::SourceLocation>& argumentLocs();
		QList<MacroExpansion*>& children();
		OOModel::MetaCallExpression* metaCall();
		MacroExpansion* xMacroParent();
		QList<MacroExpansion*>& xMacroChildren();
		Model::Node* replacementNode();

		void setMetaCall(OOModel::MetaCallExpression* metaCall);
		void setReplacementNode(Model::Node* node);

	private:
		clang::SourceRange range_;
		const clang::MacroDirective* definition_;
		MacroExpansion* parent_{};
		QList<clang::SourceLocation> argumentLocs_;
		QList<MacroExpansion*> children_;
		OOModel::MetaCallExpression* metaCall_{};
		MacroExpansion* xMacroParent_{};
		QList<MacroExpansion*> xMacroChildren_;
		Model::Node* replacementNode_{};
};

inline clang::SourceRange MacroExpansion::range() { return range_; }

inline const clang::MacroDirective* MacroExpansion::definition() { return definition_; }

inline MacroExpansion* MacroExpansion::parent() { return parent_; }

inline QList<clang::SourceLocation>& MacroExpansion::argumentLocs() { return argumentLocs_; }

inline QList<MacroExpansion*>& MacroExpansion::children() { return children_; }

inline OOModel::MetaCallExpression*MacroExpansion::metaCall() { return metaCall_; }

inline MacroExpansion*MacroExpansion::xMacroParent() { return xMacroParent_; }

inline QList<MacroExpansion*>& MacroExpansion::xMacroChildren() { return xMacroChildren_; }

inline Model::Node*MacroExpansion::replacementNode() { return replacementNode_; }

inline void MacroExpansion::setMetaCall(OOModel::MetaCallExpression* metaCall) { metaCall_ = metaCall; }

inline void MacroExpansion::setReplacementNode(Model::Node* node) { replacementNode_ = node; }

}
