/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "EnvisionPPCallbacks.h"

EnvisionPPCallbacks::EnvisionPPCallbacks(clang::SourceManager& srcManager, std::string fileName,
													  QHash<QString, QString>& attributes)
	: sourceManager_{srcManager}, fileName_{fileName}, attributes_{attributes} {}

void EnvisionPPCallbacks::MacroExpands(const clang::Token &MacroNameTok, const clang::MacroDefinition&,
													clang::SourceRange range, const clang::MacroArgs *Args)
{
	// We only care about ATTRIBUTE macros:
	if (MacroNameTok.getIdentifierInfo()->getName() == "ATTRIBUTE" ||
		 MacroNameTok.getIdentifierInfo()->getName() == "ATTRIBUTE_VALUE_CUSTOM_RETURN" ||
		 MacroNameTok.getIdentifierInfo()->getName() == "ATTRIBUTE_VALUE")
	{
		// We only care about ATTRIBUTES in the currentFile:
		if (sourceManager_.getFilename(range.getBegin()) != fileName_) return;
		unsigned numArguments = Args->getNumMacroArguments();
		Q_ASSERT(numArguments >= 3); // ATTRIBUTE macros have 3 arguments

		auto attributeName = QString::fromStdString(Args->getUnexpArgument(1u)->getIdentifierInfo()->getName().str());
		auto attributeSetter = QString::fromStdString(Args->getUnexpArgument(2u)->getIdentifierInfo()->getName().str());

		attributes_.insert(attributeName, attributeSetter);
	}
	else if (MacroNameTok.getIdentifierInfo()->getName() == "ATTRIBUTE_OOP_NAME_SYMBOL")
	{
		attributes_.insert("name", "setName");
	}
	else if (MacroNameTok.getIdentifierInfo()->getName() == "ATTRIBUTE_OOP_ANNOTATIONS")
	{
		attributes_.insert("annotations", "setAnnotations");
	}
}
