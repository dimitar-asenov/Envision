/***********************************************************************************************************************
 * ModelScene.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELSCENE_H_
#define MODELSCENE_H_

#include "visualizationbase_api.h"

#include "Scene.h"

namespace Visualization {

class ModelRenderer;

class VISUALIZATIONBASE_API ModelScene: public Scene
{
	private:
		ModelRenderer* renderer_;

	public:
		ModelScene();
		void setRenderer(ModelRenderer* renderer);

		ModelRenderer* renderer();
};

}

#endif /* MODELSCENE_H_ */
