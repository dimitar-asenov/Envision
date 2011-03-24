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

#include "VisualizationBase/headers/items/Item.h"
#include "ModelBase/headers/nodes/Node.h"

using namespace OOVisualization;
using namespace Visualization;
using namespace Model;
using namespace OOModel;

namespace CustomMethodCall {

DEFINE_EXTENSION(CustomVisualization)
REGISTER_EXTENSION_ATTRIBUTE(CustomVisualization, visName, Text, false, true, true)

QMap<QString, Visualization::ModelRenderer::ItemConstructor> CustomVisualization::visualizations;

Item* CustomVisualization::createStatement(Item* parent, Node* node)
{
	MethodCallStatement* met = static_cast<MethodCallStatement*> (node);
	CustomVisualization* custVis = met->extension<CustomVisualization>();
	if (custVis && custVis->visNameNode() && visualizations.contains(custVis->visName()))
		return visualizations.value(custVis->visName())(parent, node);
	else
		return new VMethodCallStatement(parent, met);
}

Item* CustomVisualization::createExpression(Item* parent, Node* node)
{
	MethodCallExpression* met = static_cast<MethodCallExpression*> (node);
	CustomVisualization* custVis = met->extension<CustomVisualization>();
	if (custVis && custVis->visNameNode() && visualizations.contains(custVis->visName()))
		return visualizations.value(custVis->visName())(parent, node);
	else
		return new VMethodCallExpression(parent, met);
}

void CustomVisualization::registerVisualization(const QString& name, ModelRenderer::ItemConstructor visualization)
{
	visualizations.insert(name, visualization);
}

}
