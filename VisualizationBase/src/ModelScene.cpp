/***********************************************************************************************************************
 * ModelScene.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelScene.h"
#include "ModelRenderer.h"

namespace Visualization {

ModelScene::ModelScene() : renderer_(NULL)
{
	// TODO Auto-generated constructor stub

}

void ModelScene::setRenderer(ModelRenderer* renderer)
{
	renderer_ = renderer;
}

ModelRenderer* ModelScene::renderer()
{
	return renderer_;
}

}
