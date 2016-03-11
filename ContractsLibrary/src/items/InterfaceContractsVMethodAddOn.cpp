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

#include "InterfaceContractsVMethodAddOn.h"
#include "OOVisualization/src/declarations/VMethod.h"
#include "OOVisualization/src/elements/VStatementItemList.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/typesystem/TypeArgumentBindings.h"

#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "VisualizationBase/src/Scene.h"

namespace ContractsLibrary {

InterfaceContractsVMethodAddOn::InterfaceContractsVMethodAddOn(OOModel::Method* contractsClassMethodDefinition)
: contractClassMethod_{contractsClassMethodDefinition}
{
}

QList<Visualization::Item*> InterfaceContractsVMethodAddOn::determineItems(Visualization::Item* vis,
				const QList<Visualization::Item*>& currentItems)
{
	QList<Visualization::Item*> result;
	if (!contractClassMethod_) return result;

	auto vMet = DCast<OOVisualization::VMethod>(vis);
	if ( !vMet ) return result;

	auto p = vMet->node()->parent();
	while (p && p->typeId() != OOModel::Class::typeIdStatic()) p = p->parent();
	auto cl = DCast<OOModel::Class>(p);
	if (!cl) return result;


	for (auto st : *cl->annotations())
	{
		if ( auto sti = DCast<OOModel::ExpressionStatement>(st) )
			if (auto call = DCast<OOModel::MethodCallExpression>(sti->expression()) )
				if (call->methodDefinition({}) == contractClassMethod_ && call->arguments()->size() == 1)
					if (auto ref = DCast<OOModel::ReferenceExpression> ( call->arguments()->first()) )
						if (auto contractClass = DCast<OOModel::Class> (ref->target()) )
							if (contractClass->baseClasses()->size() == 1)
								if ( auto firstBaseClassExp = DCast<OOModel::ReferenceExpression>
										(contractClass->baseClasses()->first()))
									if (firstBaseClassExp->target() == cl)
									{
										//TODO: make this work properly in case of overloading/overriding
										for (auto m : *contractClass->methods())
										{
											if (m->name() == vMet->node()->name())
											{
												auto contracts = m->items();
												if (!currentItems.isEmpty() && currentItems.first()->node() == contracts)
													return currentItems;
												else
													result.append(new OOVisualization::VStatementItemList{nullptr, contracts,
															Visualization::VList::itemStyles().get("contracts")});
												break;
											}
										}

										break;
									}
	}

	return result;
}

}
