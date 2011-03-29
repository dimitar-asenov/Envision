/***********************************************************************************************************************
 * CustomVisualization.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "CustomVisualization.h"

#include "OOVisualization/headers/statements/VMethodCallStatement.h"
#include "OOVisualization/headers/expressions/VMethodCallExpression.h"

#include "OOModel/headers/statements/MethodCallStatement.h"
#include "OOModel/headers/expressions/MethodCallExpression.h"
#include "OOModel/headers/top_level/Method.h"

#include "VisualizationBase/headers/items/Item.h"
#include "ModelBase/headers/nodes/Node.h"

using namespace OOVisualization;
using namespace Visualization;
using namespace OOModel;

namespace CustomMethodCall {

DEFINE_EXTENSION(CustomVisualization)
REGISTER_EXTENSION_ATTRIBUTE(CustomVisualization, visName, Text, false, true, true)

QMap<QString, Visualization::ModelRenderer::ItemConstructor> CustomVisualization::visualizations;

//TODO Currently it is not possible to define a MethodVisualization extension for the Method such that it does
//Different things depending on what is the actual type of node (expression call or statement call)

Item* CustomVisualization::createStatement(Item* parent, Model::Node* node)
{
	Method* met = (static_cast<MethodCallStatement*> (node))->methodDefinition();

	CustomVisualization* custVis = NULL;
	if (met) custVis = met->extension<CustomVisualization>();

	if (custVis && custVis->visNameNode() && visualizations.contains(custVis->visName()))
		return visualizations.value(custVis->visName())(parent, node);
	else
		return new VMethodCallStatement(parent, static_cast<MethodCallStatement*> (node));
}

Item* CustomVisualization::createExpression(Item* parent, Model::Node* node)
{
	Method* met = (static_cast<MethodCallExpression*> (node))->methodDefinition();

	CustomVisualization* custVis = NULL;
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
