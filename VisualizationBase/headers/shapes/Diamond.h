/***********************************************************************************************************************
 * Diamond.h
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef DIAMOND_H_
#define DIAMOND_H_

#include "../visualizationbase_api.h"

#include "Shape.h"
#include "DiamondStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API Diamond: public Shape
{
	public:
		typedef DiamondStyle StyleType;

	protected:
		qreal widthWithoutShadow;
		qreal heightWithoutShadow;
		qreal contentTop_;
		qreal contentLeft_;

		QPainterPath getDiamondPath(qreal x, qreal y, int width, int height);

	public:
		Diamond(Item *parent, StyleType *style = StyleType::getDefault());
		static const QString& className();

		StyleType* style() const;
		virtual void setStyle(ShapeStyle *style);

		virtual void update();
		virtual int contentLeft();
		virtual int contentTop();
		virtual QSize innerSize(QSize outterSize) const;
		virtual QSize outterSize(QSize innerSize) const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

inline const QString& Diamond::className() {static QString name("Diamond"); return name;}

}

#endif /* DIAMOND_H_ */
