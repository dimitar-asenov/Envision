/***********************************************************************************************************************
 * ModelRenderer.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELRENDERER_H_
#define MODELRENDERER_H_

#include "visualizationbase_api.h"

#include "ModelItem.h"
#include "ModelBase/headers/nodes/Node.h"

#include <QtCore/QVector>

namespace Visualization {

class VISUALIZATIONBASE_API ModelRenderer
{
	public:
		typedef ModelItem* (*ItemConstructor)(Item* parent, Model::Node* node);

	private:
		QVector<ItemConstructor> visualizations;

	public:
		ModelRenderer();

		ModelItem* render(Item* parent, Model::Node* node);

		void registerVisualization(int typeId, ItemConstructor visualization);
};

template<class T>
ModelItem* createVisualization(Item* parent, Model::Node* node)
{
	return new T(parent, node);
}

}

#endif /* MODELRENDERER_H_ */
