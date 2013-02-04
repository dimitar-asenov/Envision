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

/*
 * ContractsVMethodAddOn.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: Dimitar Asenov
 */

#include "ContractsVMethodAddOn.h"
#include "OOVisualization/src/top_level/VMethod.h"
#include "OOVisualization/src/elements/VStatementItemList.h"
#include "OOModel/src/top_level/Class.h"
#include "OOModel/src/top_level/Method.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"

#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "VisualizationBase/src/Scene.h"

namespace ContractsLibrary {

ContractsVMethodAddOn::ContractsVMethodAddOn(OOModel::Method* contractsClassMethodDefinition)
: contractClassMethod_(contractsClassMethodDefinition)
{
}

QList<Visualization::Item*> ContractsVMethodAddOn::determineItems(Visualization::Item* vis,
				const QList<Visualization::Item*>& currentItems)
{
	QList<Visualization::Item*> result;
	if (!contractClassMethod_) return result;

	auto vMet = dynamic_cast<OOVisualization::VMethod*>(vis);
	if ( !vMet ) return result;

	auto p = vMet->node()->parent();
	while (p && p->typeId() != OOModel::Class::typeIdStatic()) p = p->parent();
	auto cl = dynamic_cast<OOModel::Class*>(p);
	if (!cl) return result;


	for (int i = 0; i<cl->annotations()->size(); ++i)
	{
		auto st = cl->annotations()->at(i);
		if ( auto sti = dynamic_cast<OOModel::ExpressionStatement*>(st) )
			if (auto call = dynamic_cast<OOModel::MethodCallExpression*>(sti->expression()) )
				if (call->methodDefinition() == contractClassMethod_ && call->arguments()->size() == 1)
					if (auto ref = dynamic_cast<OOModel::ReferenceExpression*> ( call->arguments()->first()) )
						if (auto contractClass = dynamic_cast<OOModel::Class*> (ref->target()) )
							if (contractClass->baseClasses()->size() == 1)
								if ( auto firstBaseClassExp = dynamic_cast<OOModel::ReferenceExpression*>
										(contractClass->baseClasses()->first()))
									if (firstBaseClassExp->target() == cl)
									{
										//TODO: make this work properly in case of overloading/overriding
										for (int i = 0; i< contractClass->methods()->size(); ++i)
										{
											auto m = contractClass->methods()->at(i);
											if (m->name() == vMet->node()->name())
											{
												auto contracts = m->items();
												if (!currentItems.isEmpty() && currentItems.first()->node() == contracts)
													return currentItems;
												else
													result.append(new OOVisualization::VStatementItemList(nullptr, contracts,
															Visualization::VList::itemStyles().get("contracts")));
												break;
											}
										}

										break;
									}
	}

	return result;
}

} /* namespace ContractsLibrary */
