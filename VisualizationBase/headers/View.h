/***********************************************************************************************************************
 * View.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VIEW_H_
#define VIEW_H_

#include <QtGui/QGraphicsView>

namespace Visualization {

class Scene;

class View : public QGraphicsView
{
	public:
		View(Scene *scene, View *parent = NULL);
		virtual ~View();
};

}

#endif /* VIEW_H_ */
