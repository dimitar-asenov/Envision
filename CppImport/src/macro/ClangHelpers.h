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

namespace CppImport {

/**
 * holds the clang::SourceManager and clang::Preprocessor during macro import.
 * it provides helper methods that only depend on the SourceManager and Preprocessor.
 */
class CPPIMPORT_API ClangHelpers
{
	public:
		const clang::SourceManager* sourceManager() const;
		void setSourceManager(const clang::SourceManager* sourceManager);

		const clang::Preprocessor* preprocessor() const;
		void setPreprocessor(const clang::Preprocessor* preprocessor);

		clang::SourceLocation immediateMacroLocation(clang::SourceLocation location) const;
		void immediateSpellingHistory(clang::SourceLocation loc, QVector<clang::SourceLocation>& result) const;

		QVector<QString> argumentNames(const clang::MacroDirective* definition) const;

		bool isMacroRange(clang::SourceRange range) const;

		QString unexpandedSpelling(clang::SourceRange range) const;
		QString unexpandedSpelling(clang::SourceLocation start, clang::SourceLocation end) const;

	private:
		const clang::SourceManager* sourceManager_{};
		const clang::Preprocessor* preprocessor_{};

		clang::SourceRange getUnexpandedRange(clang::SourceRange sourceRange) const;
		QString spelling(clang::SourceRange sourceRange) const;
};

inline const clang::SourceManager* ClangHelpers::sourceManager() const { return sourceManager_; }
inline void ClangHelpers::setSourceManager(const clang::SourceManager* sourceManager)
{ sourceManager_ = sourceManager; }
inline const clang::Preprocessor* ClangHelpers::preprocessor() const { return preprocessor_; }
inline void ClangHelpers::setPreprocessor(const clang::Preprocessor* preprocessor)
{ preprocessor_ = preprocessor; }

inline bool ClangHelpers::isMacroRange(clang::SourceRange range) const
{ return range.getBegin().isMacroID() && range.getEnd().isMacroID(); }

inline QString ClangHelpers::unexpandedSpelling(clang::SourceLocation start, clang::SourceLocation end) const
{ return unexpandedSpelling(clang::SourceRange(start, end)); }

}
