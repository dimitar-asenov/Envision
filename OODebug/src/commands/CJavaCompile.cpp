/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "CJavaCompile.h"

#include "../compiler/JavaCompiler.h"
#include "ModelBase/src/model/TreeManager.h"

namespace OODebug {

CJavaCompile::CJavaCompile() : CommandWithFlags{"compile", {}, false}
{}

Interaction::CommandResult* CJavaCompile::executeNamed(Visualization::Item* source, Visualization::Item*,
	const std::unique_ptr<Visualization::Cursor>&, const QString&, const QStringList&)
{
	while (source && !source->node()) source = source->parent();
	Q_ASSERT(source);
	auto manager = source->node()->manager();
	Q_ASSERT(manager);
	auto result = JavaCompiler::compileTree(manager, "exported/" + manager->root()->symbolName());
	Q_ASSERT(result);
	return result;
}

bool CJavaCompile::canInterpret(Visualization::Item* source, Visualization::Item* target,
										  const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	if (CommandWithFlags::canInterpret(source, target, commandTokens, cursor))
	{
		// Check if there is a tree manager
		while (source && !source->node()) source = source->parent();
		return source && source->node()->manager();
	}
	return false;
}

}
