/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

class CPPIMPORT_API CppImportLogger
{
	public:
		CppImportLogger();
		~CppImportLogger();
		void setSourceManager(const clang::SourceManager* sourceManager);

		enum class Reason : int {OTHER, NOT_SUPPORTED, NO_PARENT, INSERT_PROBLEM};

		void writeError(const QString& inWhichClass, const clang::Decl* decl,
							 const Reason& r, const QString& reason = {});
		void writeWarning(const QString& inWhichClass, const clang::Decl* decl,
								const Reason& r, const QString& reason = {});
		void writeError(const QString& inWhichClass, const clang::Stmt* stmt,
							 const Reason& r, const QString& reason = {});
		void writeWarning(const QString& inWhichClass, const clang::Stmt* stmt,
								const Reason& r, const QString& reason = {});

		void writeError(const QString& inWhichClass, const clang::SourceLocation& loc,
							 const Reason& r, const QString& reason = {});

		void primitiveTypeNotSupported(const QString& typeName);
		void typeNotSupported(const clang::TypeLoc typeloc, const clang::SourceLocation& location);

		void unaryOpNotSupported(const clang::UnaryOperatorKind& kind);
		void storageClassNotSupported(const clang::StorageClass& sc);

		void overloadedOpNotSupported(const clang::OverloadedOperatorKind& kind, const bool binary);

		void outputStatistics();
	private:
		// type of output for writeOut function
		enum OUTTYPE {ERROR, WARNING};
		void writeOut(const QString& inWhichClass, const clang::Decl* decl,
						  OUTTYPE outType, const Reason& r, const QString& reason = {});
		void writeOut(const QString& inWhichClass, const clang::Stmt *stmt,
						  OUTTYPE outType, const Reason& r, const QString& reason = {});
		void initStreams();
		void printStatistic(const char* message, const QMap<QString, int>& map);

		const QString getReasonString(const Reason& r);
		// reason strings to append on maps (keep in sync with reason enum
		const QString reasons_[4] = {"_O", "_NS", "_NP", "_IP"};

		// outstreams
		QTextStream* errStream_{};
		QTextStream* warnStream_{};

		// set to true if you want output in file(s)
		bool writeToFile_{false};

		// SourceManager to get file names from clang
		const clang::SourceManager* sourceManger_{};

		QMap<QString, int> countMap_;
		QMap<QString, int> typeCountMap_;
		QMap<QString, int> unaryOpMap_;
		QMap<QString, int> overloadMap_;
		QMap<QString, int> storageMap_;
};

inline void CppImportLogger::writeError
(const QString& inWhichClass, const clang::Decl* decl, const Reason& r, const QString& reason)
{ writeOut(inWhichClass, decl, ERROR, r, reason); }

inline void CppImportLogger::writeWarning
(const QString& inWhichClass, const clang::Decl* decl, const Reason& r, const QString& reason)
{ writeOut(inWhichClass, decl, WARNING, r, reason); }

inline void CppImportLogger::writeError
(const QString& inWhichClass, const clang::Stmt* stmt, const Reason& r, const QString& reason)
{ writeOut(inWhichClass, stmt, ERROR, r, reason); }

inline void CppImportLogger::writeWarning
(const QString& inWhichClass, const clang::Stmt* stmt, const Reason& r, const QString& reason)
{ writeOut(inWhichClass, stmt, WARNING, r, reason); }

}
