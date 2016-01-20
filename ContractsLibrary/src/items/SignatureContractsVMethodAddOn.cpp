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

#include "SignatureContractsVMethodAddOn.h"
#include "OOVisualization/src/declarations/VMethod.h"
#include "OOVisualization/src/elements/VStatementItemList.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"

#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "VisualizationBase/src/Scene.h"

namespace ContractsLibrary {

SignatureContractsVMethodAddOn::SignatureContractsVMethodAddOn(OOModel::Method* contractsClassMethodDefinition)
: contractClassMethod_{contractsClassMethodDefinition}
{
}

QList<Visualization::Item*> SignatureContractsVMethodAddOn::determineItems(Visualization::Item* vis,
				const QList<Visualization::Item*>& currentItems)
{
	QList<Visualization::Item*> result;
	if (!contractClassMethod_) return result;

	auto vMet = DCast<OOVisualization::VMethod>(vis);
	if ( !vMet ) return result;

	if (!currentItems.isEmpty() && currentItems.first()->node() == vMet->node()->items())
		return currentItems;

	result.append(new OOVisualization::VStatementItemList{nullptr, vMet->node()->items(),
																Visualization::VList::itemStyles().get("contracts")});
	return result;
}

}
