/***********************************************************************************************************************
 * View.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VIEW_H_
#define VIEW_H_

#include "../visualizationbase_api.h"

#include <QtGui/QGraphicsView>

namespace Visualization {

class Scene;

class VISUALIZATIONBASE_API View : public QGraphicsView
{
	public:
		View(Scene *scene, View *parent = NULL);
		virtual ~View();
		QRectF visibleRect();
		Scene* scene();
};

}

#endif /* VIEW_H_ */
