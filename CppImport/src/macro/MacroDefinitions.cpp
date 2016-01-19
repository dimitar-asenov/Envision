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
#include "../ClangHelpers.h"

#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/declarations/Project.h"

namespace CppImport {

MacroDefinitions::MacroDefinitions(ClangHelpers& clang) : clang_{clang}
{
	if (directoryToNamespaceMap_.isEmpty())
	{
		directoryToNamespaceMap_.insert("ModelBase", "Model");
		directoryToNamespaceMap_.insert("VisualizationBase", "Visualization");
		directoryToNamespaceMap_.insert("InteractionBase", "Interaction");
	}
}

QString MacroDefinitions::definitionName(const clang::MacroDirective* md) const
{
	auto it = definitions_.find(md);

	return it != definitions_.end() ? *it : nullptr;
}

QString MacroDefinitions::signature(const clang::MacroDirective* md) const
{
	QString namespaceName, fileName;

	auto parentProject = clang_.projectForLocation(md->getLocation());
	if (parentProject != clang_.rootProject())
		return parentProject->name() + "/" + definitionName(md);
	else
		return "/ExternalMacro/" + definitionName(md);
}

OOModel::ReferenceExpression* MacroDefinitions::expansionQualifier(const clang::MacroDirective* md) const
{
	auto parentProject = clang_.projectForLocation(md->getLocation());
	if (parentProject == clang_.rootProject())
		return new OOModel::ReferenceExpression{"ExternalMacro"};

	return {};
}

}
