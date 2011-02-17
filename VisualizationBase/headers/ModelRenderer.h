/***********************************************************************************************************************
 * ModelRenderer.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELRENDERER_H_
#define MODELRENDERER_H_

#include "visualizationbase_api.h"
#include "ModelBase/headers/nodes/Node.h"

#include <QtCore/QVector>

namespace Visualization {

class Item;

class VISUALIZATIONBASE_API ModelRenderer
{
	public:
		typedef Item* (*ItemConstructor)(Item* parent, Model::Node* node);

		ModelRenderer();
		virtual ~ModelRenderer();

		virtual Item* render(Item* parent, Model::Node* node);

		void registerVisualization(int typeId, ItemConstructor visualization);
		void setUseDefaultVisualizations(bool useDefault);

	private:
		QVector<ItemConstructor> visualizations;

		bool useDefaultVisualizations_;
};

inline void ModelRenderer::setUseDefaultVisualizations(bool useDefault)
{
	useDefaultVisualizations_ = useDefault;
}

template<class VIS, class NODE>
Item* createVisualization(Item* parent, Model::Node* node)
{
	return new VIS(parent, static_cast<NODE*> (node));
}



}

#endif /* MODELRENDERER_H_ */
