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

namespace Visualization {

class SceneHandlerItem;
class SelectedItem;

class VISUALIZATIONBASE_API Scene : public QGraphicsScene
{
	public:
		Scene();
		virtual ~Scene();
		void setRenderer(ModelRenderer* renderer);

		ModelRenderer* renderer();
		static ModelRenderer* defaultRenderer();

		void addTopLevelItem(Item* item);
		void removeTopLevelItem(Item* item);
		void scheduleUpdate();

		virtual void customEvent(QEvent *event);

		virtual SceneHandlerItem* sceneHandlerItem();

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
