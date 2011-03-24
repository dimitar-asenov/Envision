/***********************************************************************************************************************
 * CustomVisualization.h
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CUSTOMVISUALIZATION_H_
#define CUSTOMVISUALIZATION_H_

#include "custommethodcall_api.h"

#include "VisualizationBase/headers/ModelRenderer.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace Visualization {
	class Item;
}

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API CustomVisualization
{
	DECLARE_EXTENSION(CustomVisualization)

	EXTENSION_ATTRIBUTE_VALUE_CUSTOM_RETURN(::Model::Text, visName, setVisName, QString, const QString&)

	public:
		static Visualization::Item* createStatement(Visualization::Item* parent, Model::Node* node);
		static Visualization::Item* createExpression(Visualization::Item* parent, Model::Node* node);
		static void registerVisualization(const QString& name, Visualization::ModelRenderer::ItemConstructor visualization);

	private:
		static QMap<QString, Visualization::ModelRenderer::ItemConstructor> visualizations;
};

}

#endif /* CUSTOMVISUALIZATION_H_ */
