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
 * Scene.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SCENE_H_
#define SCENE_H_

#include "visualizationbase_api.h"
#include "ModelRenderer.h"

#include <QtGui/QGraphicsScene>

namespace Model {
	class Model;
	class Node;
}

namespace Visualization {

class SceneHandlerItem;
class SelectedItem;

class VISUALIZATIONBASE_API Scene : public QGraphicsScene
{
	Q_OBJECT

	private:

		// This is needed in order to make the Signals and Slots mechanism work. Otherwise we are not able to connect to
		// the signal provided from Model. This is because the signatures of the two methods, must match exactly
		// (stringwise).
		typedef Model::Node Node;

	public:
		Scene();
		virtual ~Scene();
		void setRenderer(ModelRenderer* renderer);

		ModelRenderer* renderer();
		static ModelRenderer* defaultRenderer();

		void addTopLevelItem(Item* item);
		void removeTopLevelItem(Item* item);
		void scheduleUpdate();
		void listenToModel(Model::Model* model);

		virtual void customEvent(QEvent *event);

		virtual SceneHandlerItem* sceneHandlerItem();

	public slots:
		void nodesUpdated(QList<Node*> nodes);

	protected:
		bool event(QEvent *event);

	private:
		bool needsUpdate;

		ModelRenderer* renderer_;
		SceneHandlerItem* sceneHandlerItem_;
		QList<Item*> topLevelItems;
		QList<SelectedItem*> selections;

		static ModelRenderer defaultRenderer_;
};

inline void Scene::setRenderer(ModelRenderer* renderer) { renderer_ = renderer; }
inline ModelRenderer* Scene::renderer() { return renderer_? renderer_ : (&defaultRenderer_); }
inline ModelRenderer* Scene::defaultRenderer() { return &defaultRenderer_; }
inline SceneHandlerItem* Scene::sceneHandlerItem() {return sceneHandlerItem_; }


}

#endif /* SCENE_H_ */
