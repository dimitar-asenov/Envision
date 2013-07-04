/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "CppImportLogger.h"

namespace CppImport {

CppImportLogger::CppImportLogger()
{
	initStreams();
}

CppImportLogger::~CppImportLogger()
{
	SAFE_DELETE(errStream_);
	SAFE_DELETE(warnStream_);
}

void CppImportLogger::setSourceManager(const clang::SourceManager* sourceManager)
{
	Q_ASSERT(sourceManager);
	sourceManger_ = sourceManager;
}

void CppImportLogger::writeOut(const QString& inWhichClass, const QString& reason, const clang::NamedDecl* decl,
										 CppImportLogger::OUTTYPE outType)
{
	QTextStream* outStream;
	switch(outType)
	{
		case ERROR: outStream = errStream_; break;
		case WARNING: outStream = warnStream_; break;
		default: return;
	}

	QString clangType = QString(decl->getDeclKindName());

	int newCount = countMap_.value(clangType) + 1;
	countMap_.insert(clangType, newCount);

	std::pair<clang::FileID,unsigned> decomposedLoc = sourceManger_->getDecomposedLoc(decl->getLocation());
	(*outStream) << "ERR/WARN: \t In class : " << inWhichClass << " \n\t reason : " << reason
					 << " \n\t in clang node : " << clangType
					 << " \n\t clang node name : " << QString::fromStdString(decl->getNameAsString())
					 << " \n\t in file : " << sourceManger_->getBufferName(decl->getLocation())
					 << " \n\t on line : " << sourceManger_->getLineNumber(decomposedLoc.first,decomposedLoc.second)
					 << "\n";

}

void CppImportLogger::writeOut(const QString& inWhichClass, const QString& reason, const clang::Stmt* stmt,
										 CppImportLogger::OUTTYPE outType)
{
	if(!stmt)
		return;
	QTextStream* outStream;
	switch(outType)
	{
		case ERROR: outStream = errStream_; break;
		case WARNING: outStream = warnStream_; break;
		default: return;
	}
	QString clangType = QString(stmt->getStmtClassName());

	int newCount = countMap_.value(clangType) + 1;
	countMap_.insert(clangType, newCount);

	std::pair<clang::FileID,unsigned> decomposedLoc = sourceManger_->getDecomposedLoc(stmt->getLocStart());
	(*outStream) << "ERR/WARN: \t In class : " << inWhichClass << " \n\t reason : " << reason
					 << " \n\t in stmt class node : " << clangType
					 << " \n\t in file : " << sourceManger_->getBufferName(stmt->getLocStart())
					 << " \n\t on line : " << sourceManger_->getLineNumber(decomposedLoc.first,decomposedLoc.second)
					 << "\n";

}

void CppImportLogger::writeError(const QString& inWhichClass, const QString& reason, const clang::SourceLocation& loc)
{
	std::pair<clang::FileID,unsigned> decomposedLoc = sourceManger_->getDecomposedLoc(loc);
	(*errStream_) << "ERR/WARN: \t In class : " << inWhichClass << " \n\t reason : " << reason
					  << " \n\t in file : " << sourceManger_->getBufferName(loc)
					  << " \n\t on line : " << sourceManger_->getLineNumber(decomposedLoc.first,decomposedLoc.second)
					  << "\n";
}

void CppImportLogger::typeNotSupported(const QString& typeName)
{
	int newCount = typeCountMap_.value(typeName) + 1;
	typeCountMap_.insert(typeName, newCount);
}

void CppImportLogger::unaryOpNotSupported(const clang::UnaryOperatorKind& kind)
{
	QString missing;
	switch(kind)
	{
		case clang::UO_Real: missing = "UO_Real"; break;
		case clang::UO_Imag: missing = "UO_Imag"; break;
		case clang::UO_Extension: missing = "UO_Extension"; break;
		default: missing = "Uknown unary op"; break;
	}

	int newCount = unaryOpMap_.value(missing) + 1;
	unaryOpMap_.insert(missing,newCount);
}

void CppImportLogger::storageClassNotSupported(const clang::StorageClass& sc)
{
	QString missing;
	switch(sc)
	{
		case clang::SC_Extern: missing = "SC_Extern"; break;
		case clang::SC_PrivateExtern: missing = "SC_PrivateExtern"; break;
		case clang::SC_OpenCLWorkGroupLocal: missing = "SC_OpenCLWorkGroupLocal"; break;
		case clang::SC_Auto: missing = "SC_Auto"; break;
		case clang::SC_Register: missing = "SC_Register"; break;
		default: missing = "uknown sc"; break;
	}

	int newCount = storageMap_.value(missing) + 1;
	storageMap_.insert(missing,newCount);
}

void CppImportLogger::overloadedOpNotSupported(const clang::OverloadedOperatorKind& kind, const bool binary)
{
	QString missing;
	switch(kind)
	{
		case clang::OO_New: missing = "New"; break;
		case clang::OO_Delete: missing = "Delete"; break;
		case clang::OO_Array_New: missing = "Array_New"; break;
		case clang::OO_Array_Delete: missing = "Array_Delete"; break;
		case clang::OO_Comma: missing = "Comma"; break;
		case clang::OO_Conditional: missing = "Conditional"; break;
		default:
			missing = "Uknown_OVERLOAD";
			break;
	}
	binary ? missing.append("_BIN") : missing.append("_UN");

	int newCount = overloadMap_.value(missing) + 1;
	overloadMap_.insert(missing, newCount);
}

void CppImportLogger::initStreams()
{
	if(writeToFile_)
	{
		std::cout << "LOGGER ERR: NOT YET SUPPORTED TO WRITE TO FILE" << std::endl;
	}
	else
	{
		errStream_ = new QTextStream(stderr);
		warnStream_ = new QTextStream(stdout);
	}
}

void CppImportLogger::outputStatistics()
{
	// intro section
	(*warnStream_) << endl;
	(*warnStream_) << qSetFieldWidth(36) << center << qSetPadChar('=')
						<< "Statistics of warnings and errors" << reset << endl;

	printStatistic("General errors", countMap_);
	printStatistic("Types not supported by envision", typeCountMap_);
	printStatistic("Unary operations not supported", unaryOpMap_);
	printStatistic("Overloaded operations not supported", overloadMap_);
	printStatistic("Storage class specifiers not supported", storageMap_);

	// end section
	(*warnStream_) << endl;
	(*warnStream_) << qSetFieldWidth(36) << center << qSetPadChar('=')
						<< "Statistics End" << reset << endl;
}

void CppImportLogger::printStatistic(const char* message, const QMap<QString, int>& map)
{
	if(!map.empty())
	{
		(*warnStream_) << endl << message << endl;
		for(auto iter = map.constBegin(); iter != map.constEnd(); ++iter)
			(*warnStream_) << left << qSetFieldWidth(30) << iter.key() + ":"
								<< right << qSetFieldWidth(6) << iter.value() << endl;
	}
}

}
