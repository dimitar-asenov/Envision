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

CppImportLogger::CppImportLogger(clang::SourceManager* sourceManager) :
	sourceManger_(sourceManager)
{
	initStreams();
}

CppImportLogger::~CppImportLogger()
{
	delete errStream_;
	delete warnStream_;
}

void CppImportLogger::writeOut(QString &inWhichClass, QString &reason, QString &clangType, clang::NamedDecl* decl, CppImportLogger::OUTTYPE outType)
{
	QTextStream* outStream;
	switch(outType)
	{
		case ERROR:
			outStream = errStream_;
			break;
		case WARNING:
			outStream = warnStream_;
			break;
		default:
			return;
	}

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

void CppImportLogger::writeOut(QString &inWhichClass, QString &reason, QString &clangType, clang::Stmt* stmt, CppImportLogger::OUTTYPE outType)
{
	QTextStream* outStream;
	switch(outType)
	{
		case ERROR:
			outStream = errStream_;
			break;
		case WARNING:
			outStream = warnStream_;
			break;
		default:
			return;
	}

	int newCount = countMap_.value(clangType) + 1;
	countMap_.insert(clangType, newCount);

	std::pair<clang::FileID,unsigned> decomposedLoc = sourceManger_->getDecomposedLoc(stmt->getLocStart());
	(*outStream) << "ERR/WARN: \t In class : " << inWhichClass << " \n\t reason : " << reason
					 << " \n\t in clang node : " << clangType
						 // TODO maybe output something more useful
					 << " \n\t in stmt class node : " << stmt->getStmtClassName()
					 << " \n\t in file : " << sourceManger_->getBufferName(stmt->getLocStart())
					 << " \n\t on line : " << sourceManger_->getLineNumber(decomposedLoc.first,decomposedLoc.second)
					 << "\n";

}

void CppImportLogger::typeNotSupported(QString typeName)
{
	int newCount = typeCountMap_.value(typeName) + 1;
	typeCountMap_.insert(typeName, newCount);
}

void CppImportLogger::unaryOpNotSupported(clang::UnaryOperatorKind kind)
{
	QString missing;
	switch(kind)
	{
		case clang::UO_Real:
			missing = "UO_Real";
			break;
		case clang::UO_Imag:
			missing = "UO_Imag";
			break;
		case clang::UO_Extension:
			missing = "UO_Extension";
			break;
		default:
			missing = "Uknown unary op";
			break;
	}

	int newCount = unaryOpMap_.value(missing) + 1;
	unaryOpMap_.insert(missing,newCount);
}

void CppImportLogger::binaryOpNotSupported(clang::BinaryOperatorKind kind)
{
	QString missing;
	switch(kind)
	{
		case clang::BO_PtrMemD:
			missing = "BO_PtrMemD";
			break;
		case clang::BO_PtrMemI:
			missing = "BO_PtrMemI";
			break;
		case clang::BO_Comma:
			missing = "BO_Comma";
			break;
		default:
			missing = "uknown binop";
			break;
	}

	int newCount = binOpMap_.value(missing) + 1;
	binOpMap_.insert(missing,newCount);
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

	// general errors
	if(!countMap_.empty())
		for(auto iter = countMap_.constBegin(); iter != countMap_.constEnd(); ++iter)
			(*warnStream_) << left << qSetFieldWidth(30) << iter.key() + ":"
								<< right << qSetFieldWidth(6) << iter.value() << endl;

	// types section
	(*warnStream_) << "\nTypes not supported by envision:\n";
	if(!typeCountMap_.empty())
		for(auto it = typeCountMap_.constBegin(); it != typeCountMap_.constEnd(); ++it)
			(*warnStream_) << left << qSetFieldWidth(30) << it.key() + ":"
								<< right << qSetFieldWidth(6) << it.value() << endl;
	// operators section
	(*warnStream_) << "\nBinary operations not supported:\n";
	if(!binOpMap_.empty())
		for(auto it = binOpMap_.constBegin(); it!= binOpMap_.constEnd(); ++it)
			(*warnStream_) << left << qSetFieldWidth(30) << it.key() + ":"
								<< right << qSetFieldWidth(6) << it.value() << endl;

	(*warnStream_) << "\nUnary operations not supported:\n";
	if(!unaryOpMap_.empty())
		for(auto it = unaryOpMap_.constBegin(); it!= unaryOpMap_.constEnd(); ++it)
			(*warnStream_) << left << qSetFieldWidth(30) << it.key() + ":"
								<< right << qSetFieldWidth(6) << it.value() << endl;

	// end section
	(*warnStream_) << endl;
	(*warnStream_) << qSetFieldWidth(36) << center << qSetPadChar('=')
						<< "Statistics End" << reset << endl;
}

}
