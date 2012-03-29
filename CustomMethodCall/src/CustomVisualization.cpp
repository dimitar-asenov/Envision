/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * CustomVisualization.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "CustomVisualization.h"

#include "OOVisualization/src/expressions/VMethodCallExpression.h"

#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/top_level/Method.h"

#include "VisualizationBase/src/items/Item.h"
#include "ModelBase/src/nodes/Node.h"

using namespace OOVisualization;
using namespace Visualization;
using namespace OOModel;

namespace CustomMethodCall {

DEFINE_EXTENSION(CustomVisualization)
REGISTER_EXTENSION_ATTRIBUTE(CustomVisualization, visName, Text, false, true, true)

QMap<QString, Visualization::ModelRenderer::ItemConstructor> CustomVisualization::visualizations;

//TODO Currently it is not possible to define a MethodVisualization extension for the Method such that it does
//Different things depending on what is the actual type of node (expression call or statement call)

Item* CustomVisualization::createExpression(Item* parent, Model::Node* node)
{
	Method* met = (static_cast<MethodCallExpression*> (node))->methodDefinition();

	CustomVisualization* custVis = nullptr;
	if (met) custVis = met->extension<CustomVisualization>();

	if (custVis && custVis->visNameNode() && visualizations.contains(custVis->visName()))
		return visualizations.value(custVis->visName())(parent, node);
	else
		return new VMethodCallExpression(parent, static_cast<MethodCallExpression*> (node));
}

void CustomVisualization::registerVisualization(const QString& name, ModelRenderer::ItemConstructor visualization)
{
	visualizations.insert(name, visualization);
}

}
