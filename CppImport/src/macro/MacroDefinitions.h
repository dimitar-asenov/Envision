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

namespace OOModel {
	class ReferenceExpression;
}

namespace CppImport {

class ClangHelpers;

/**
 * stores the raw macro definition (not MetaDefinition) information provided by PPCallback for a translation unit.
 */
class CPPIMPORT_API MacroDefinitions
{
	public:
		MacroDefinitions(ClangHelpers& clang);

		void addMacroDefinition(const QString& name, const clang::MacroDirective* md);

		QString definitionName(const clang::MacroDirective* md) const;

		/**
		 * return whether md defines a begin incomplete macro.
		 */
		bool isPartialBegin(const clang::MacroDirective* md) const;

		/**
		 * return whether md defines an end incomplete macro.
		 */
		bool isPartialEnd(const clang::MacroDirective* md) const;

		QString signature(const clang::MacroDirective* md) const;

		void clear();

	private:
		ClangHelpers& clang_;
		QHash<const clang::MacroDirective*, QString> definitions_;

		QHash<QString, QString> directoryToNamespaceMap_;

};

inline void MacroDefinitions::addMacroDefinition(const QString& name, const clang::MacroDirective* md)
{
	definitions_[md] = name;
}

inline bool MacroDefinitions::isPartialBegin(const clang::MacroDirective* md) const
{
	return definitionName(md).startsWith("BEGIN_");
}

inline bool MacroDefinitions::isPartialEnd(const clang::MacroDirective* md) const
{
	return definitionName(md).startsWith("END_");
}

inline void MacroDefinitions::clear() { definitions_.clear(); }

}
