/***********************************************************************************************************************
 * MiniMap.h
 *
 *  Created on: Jan 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MINIMAP_H_
#define MINIMAP_H_

#include "../visualizationbase_api.h"

#include "View.h"

namespace Visualization {

class Scene;

class VISUALIZATIONBASE_API MiniMap : public View
{
	Q_OBJECT

	public:
		MiniMap(Scene *scene, View *parent);

		void updatePosition();

		void setMargin(qreal margin);

		static const qreal DEFAULT_MARGIN = 2.0;

	public slots:
		void sceneRectChanged(const QRectF & rect);
		void visibleRectChanged();

	protected:
		virtual void resizeEvent(QResizeEvent *event);
		virtual void paintEvent(QPaintEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);

	private:
		View *parent;
		int margin;

		QRectF sceneRect;
		QRectF visibleRect;
		QRect drawnRect;

		void updateMap();

};

inline void MiniMap::setMargin(qreal margin_) { margin = margin_; }

}

#endif /* MINIMAP_H_ */
