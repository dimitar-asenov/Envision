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

#include "MainMethodFinder.h"

#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/expressions/types/ClassTypeExpression.h"
#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/elements/Modifier.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/types/PrimitiveType.h"

namespace OODebug {

static bool isVoidReturnType(OOModel::Method *m)
{
	if (auto results = m->results())
		if (results->size() == 1)
			if (auto resultType = results->at(0))
				if (auto primitiveType = DCast<OOModel::PrimitiveTypeExpression>(resultType->typeExpression()))
					return OOModel::PrimitiveType::VOID == primitiveType->typeValue();
	return false;
}

void MainMethodFinder::init()
{
	addType<OOModel::Method>(visitMethod);
}

OOModel::Method* MainMethodFinder::visitChildren(Model::Node* n)
{
	auto children = n->children();
	auto it = children.begin();
	if (!children.isEmpty())
	{
		while (true)
		{
			// Return the first match
			if (it+1 == children.end()) return visit(*it);
			else if (auto ret = visit(*it))
				return ret;
			++it;
		}
	}

	// Return default value otherwise.
	return nullptr;
}

OOModel::Method* MainMethodFinder::visitMethod(MainMethodFinder*, OOModel::Method* m)
{
	static auto expectedModifier = OOModel::Modifier{OOModel::Modifier::Public | OOModel::Modifier::Static}.get();
	Q_ASSERT(m->modifiers());
	if (!QString{"main"}.compare(m->name()) && m->modifiers()->get() == expectedModifier && m->arguments())
		if (m->arguments()->size() == 1)
			if (auto firstArgument = m->arguments()->at(0))
				if (auto arrayType = DCast<OOModel::ArrayTypeExpression>(firstArgument->typeExpression()))
					if (auto refType = DCast<OOModel::ReferenceExpression>(arrayType->typeExpression()))
						if (!QString{"String"}.compare(refType->name()) && isVoidReturnType(m))
							return m;
	return nullptr;
}

}
