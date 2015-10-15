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

#include "MacroDefinitions.h"

namespace CppImport {

MacroDefinitions::MacroDefinitions(const ClangHelpers& clang) : clang_(clang) {}

QString MacroDefinitions::definitionName(const clang::MacroDirective* md) const
{
	auto it = definitions_.find(md);

	return it != definitions_.end() ? *it : nullptr;
}

bool MacroDefinitions::macroDefinitionLocation(const clang::MacroDirective* md, QString& namespaceName,
																QString& containerName) const
{
	auto presumedLocation = clang_.sourceManager()->getPresumedLoc(md->getMacroInfo()->getDefinitionLoc());
	auto path = QDir(presumedLocation.getFilename()).absolutePath();

	/*
	 * given a path: ../Envision/A/../B.xyz
	 * this regex captures A (index 1) and B.xyz (index 3)
	 */
	QRegularExpression regex ("/Envision/(\\w+)(/.*/|/)(\\w+\\.\\w+)$", QRegularExpression::DotMatchesEverythingOption);
	auto match = regex.match(path);
	if (!match.hasMatch()) return false;

	// use the directory name of A as the namespace name
	namespaceName = match.captured(1);

	// some Envision namespaces don't have the same name as the corresponding directory
	if (namespaceName == "ModelBase")
		namespaceName = "Model";
	else if (namespaceName == "VisualizationBase")
		namespaceName = "Visualization";
	else if (namespaceName == "InteractionBase")
		namespaceName = "Interaction";
	else if (namespaceName == "AlloyIntegration")
		namespaceName = "Alloy";
	else if (namespaceName == "HelloWorld")
		namespaceName = "Hello";

	/*
	 * the container name should be equal to B therefore we remove .h
	 * in case of .cpp we append _CPP to the containerName to avoid ambiguity
	 */
	containerName = match.captured(3).replace(".h", "").replace(".cpp", "_CPP");

	return true;
}

QString MacroDefinitions::hash(const clang::MacroDirective* md) const
{
	QString namespaceName, fileName;

	if (macroDefinitionLocation(md, namespaceName, fileName))
		return namespaceName + "/" + fileName + "/" + definitionName(md);
	else
		return "/ExternalMacro/" + definitionName(md);
}

OOModel::ReferenceExpression* MacroDefinitions::expansionQualifier(const clang::MacroDirective* md) const
{
	QString namespaceName, fileName;

	if (macroDefinitionLocation(md, namespaceName, fileName))
		return new OOModel::ReferenceExpression(fileName, new OOModel::ReferenceExpression(namespaceName));
	else
		return new OOModel::ReferenceExpression("ExternalMacro");
}

}
