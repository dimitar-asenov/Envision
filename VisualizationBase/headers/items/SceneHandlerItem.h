/***********************************************************************************************************************
 * SceneHandlerItem.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SCENEHANDLERITEM_H_
#define SCENEHANDLERITEM_H_

#include "../visualizationbase_api.h"

#include "Item.h"

namespace Visualization {

class Scene;

class VISUALIZATIONBASE_API SceneHandlerItem : public Item
{
	public:
		SceneHandlerItem(Scene* scene);

		virtual InteractionHandler* handler() const;
		static void setInteractionHandler(InteractionHandler* handler);

	protected:
		virtual void determineChildren();
		virtual void updateState();

	private:
		static InteractionHandler* handler_;
};

inline InteractionHandler* SceneHandlerItem::handler() const { return handler_; }
inline void SceneHandlerItem::setInteractionHandler(InteractionHandler* handler) { handler_ = handler; }
}

#endif /* SCENEHANDLERITEM_H_ */
