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

#include "ModelBase/src/nodes/TypedList.hpp"

#include "../CodeGenerationVisitor.h"

#include "OOModel/src/declarations/Declaration.h"

template class Model::TypedList<OOModel::MetaCallExpression>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(MetaCallExpression)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(MetaCallExpression)

REGISTER_ATTRIBUTE(MetaCallExpression, callee, Expression, false, false, true)
REGISTER_ATTRIBUTE(MetaCallExpression, arguments, List, false, false, true)
REGISTER_ATTRIBUTE(MetaCallExpression, cache, Node, false, true, false)

MetaCallExpression::MetaCallExpression(const QString& name, Expression* referencePrefix)
: Super(nullptr, MetaCallExpression::getMetaData())
{
	setCallee(new ReferenceExpression{name, referencePrefix});
}

MetaDefinition* MetaCallExpression::metaDefinition()
{
	MetaDefinition* ret = nullptr;

	auto calleeType = callee()->type();
	if (auto spt = dynamic_cast<SymbolProviderType*>(calleeType.get()))
	{
		if (auto m = DCast<MetaDefinition>(spt->symbolProvider()))
			ret = m;
	}

	return ret;
}

void MetaCallExpression::bindMetaCalls(Model::Node* n, MetaBinding* binding)
{
	if (auto metaCall = DCast<MetaCallExpression>(n))
		if (auto callee = DCast<ReferenceExpression>(metaCall->callee()))
			for (auto i = 0; i < binding->mappings()->size(); i++)
				if (callee->name() == binding->mappings()->at(i)->name())
					callee->setName(binding->mappings()->at(i)->value()->name());

	for (auto child : n->children()) bindMetaCalls(child, binding);
}

Declaration* MetaCallExpression::generatedTree()
{
	// only generate tree if the cache is empty
	if (!cache())
	{
		auto metaDef = metaDefinition();
		if (!metaDef) return nullptr;

		// check if the real context of this meta call matches the intended usage context
		auto realContext = this->firstAncestorOfType<Declaration>();
		if (metaDef->context()->typeId() != realContext->typeId())
		{
			qDebug() << "context"
						<< metaDef->context()->typeName()
						<< "does not match usage"
						<< realContext->typeName();
			return nullptr;
		}

		if (arguments()->size() != metaDef->arguments()->size())
		{
			qDebug() << "#metaDefArgs != #metaCallArgs";
			return nullptr;
		}

		// prepare visitor arguments using meta call arguments
		QMap<QString, Model::Node*> args;
		for (auto i = 0; i < arguments()->size(); i++)
			args.insert(metaDef->arguments()->at(i)->name(), arguments()->at(i));

		// use meta bindings to add additional visitor arguments
		for (auto i = 0; i < metaDef->metaBindings()->size(); i++)
		{
			auto metaBinding = metaDef->metaBindings()->at(i);

			if (auto input = args[metaBinding->input()->name()])
			{
				auto cloned = input->clone();
				bindMetaCalls(cloned, metaBinding);
				args.insert(metaBinding->name(), cloned);
			}
		}

		bool notifyModification = (manager() && !manager()->canBeModified(this));

		if (notifyModification) this->beginModification("code generation");

		// stage 0: clone meta definition
		// cache generated tree already at this point to enable reference resolution
		setCache(metaDef->context()->clone());

		// stage 1: perform code generation
		CodeGenerationVisitor codeGenVisitor (args);
		codeGenVisitor.visit(cache());

		// stage 2: merge generated code
		// TODO: implementation

		if (notifyModification) this->endModification();
	}

	return static_cast<Declaration*>(cache());
}

}
