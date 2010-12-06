/***********************************************************************************************************************
 * Scene.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SCENE_H_
#define SCENE_H_

#include <QtGui/QGraphicsScene>

namespace Visualization {

class Scene : public QGraphicsScene
{
	public:
		Scene();
		virtual ~Scene();
};

}

#endif /* SCENE_H_ */
