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

namespace CppImport {

class CPPIMPORT_API ClangHelpers
{
	public:
		void setSourceManager(const clang::SourceManager* sourceManager);
		void setPreprocessor(const clang::Preprocessor* preprocessor);

		const clang::SourceManager* sourceManager() const;

		QString spelling(clang::SourceRange range) const;
		QString spelling(clang::SourceLocation location) const;
		QString spelling(clang::SourceLocation start, clang::SourceLocation end) const;

		clang::SourceLocation immediateMacroLocation(clang::SourceLocation location) const;
		void immediateSpellingHistory(clang::SourceLocation loc, QVector<clang::SourceLocation>* result) const;

		QVector<QString> argumentNames(const clang::MacroDirective* definition) const;

		bool isMacroRange(clang::SourceRange range) const;

	private:
		const clang::Preprocessor* preprocessor_{};
		const clang::SourceManager* sourceManager_{};
};

inline void ClangHelpers::setSourceManager(const clang::SourceManager* sourceManager)
{ sourceManager_ = sourceManager; }

inline void ClangHelpers::setPreprocessor(const clang::Preprocessor* preprocessor) { preprocessor_ = preprocessor; }

inline QString ClangHelpers::spelling(clang::SourceLocation loc) const { return spelling(loc, loc); }

inline bool ClangHelpers::isMacroRange(clang::SourceRange range) const
{ return range.getBegin().isMacroID() && range.getEnd().isMacroID(); }

inline QString ClangHelpers::spelling(clang::SourceRange range) const
{ return spelling(range.getBegin(), range.getEnd()); }

inline const clang::SourceManager* ClangHelpers::sourceManager() const { return sourceManager_; }

}
