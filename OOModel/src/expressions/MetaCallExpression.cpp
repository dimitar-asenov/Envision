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

#include "MetaCallExpression.h"

#include "ReferenceExpression.h"
#include "../types/SymbolProviderType.h"
#include "../declarations/MetaDefinition.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"

#include "CodeGenerationVisitor.h"

DEFINE_TYPED_LIST(OOModel::MetaCallExpression)

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(MetaCallExpression)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(MetaCallExpression)

REGISTER_ATTRIBUTE(MetaCallExpression, callee, Expression, false, false, true)
REGISTER_ATTRIBUTE(MetaCallExpression, arguments, TypedListOfExpression, false, false, true)

MetaCallExpression::MetaCallExpression(const QString& name, Expression* referencePrefix)
: Super(nullptr, MetaCallExpression::getMetaData())
{
	setCallee(new ReferenceExpression(name, referencePrefix));
}

MetaDefinition* MetaCallExpression::metaDefinition()
{
	MetaDefinition* ret = nullptr;

	auto calleeType = callee()->type();
	if (auto spt = dynamic_cast<SymbolProviderType*>(calleeType))
	{
		if (auto m = dynamic_cast<MetaDefinition*>(spt->symbolProvider()))
			ret = m;
	}

	return ret;
}

Declaration* MetaCallExpression::generate()
{
	auto metaDef = metaDefinition();
	if (!metaDef) return nullptr;

	// every formal argument must have a value
	if (arguments()->size() != metaDef->arguments()->size())
	{
		qDebug() << "#metaDefArgs != #metaCallArgs";
		return nullptr;
	}

	QMap<QString, Model::Node*> args;
	for (auto i = 0; i < arguments()->size(); i++)
	{
		args.insert(metaDef->arguments()->at(i)->name(), arguments()->at(i));
	}

	auto cloned = metaDef->context()->clone();

	CodeGenerationVisitor codeGenVisitor (args);
	codeGenVisitor.visit(cloned);

	return cloned;
}

}
