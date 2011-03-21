/***********************************************************************************************************************
 * Braces.h
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BRACES_H_
#define BRACES_H_

#include "../visualizationbase_api.h"

#include "Shape.h"
#include "BracesStyle.h"

#include <QtCore/QPointF>

namespace Visualization {

class VISUALIZATIONBASE_API Braces: public Shape
{
	public:
		typedef BracesStyle StyleType;

		Braces(Item *parent, StyleType *style = StyleType::getDefault());
		static const QString& className();

		BracesStyle* style() const;
		virtual void setStyle(ShapeStyle *style);

		virtual void update();
		virtual int contentLeft();
		virtual int contentTop();

		virtual QSize innerSize(QSize outterSize) const;
		virtual QSize outterSize(QSize innerSize) const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	protected:
		qreal contentTop_;
		qreal contentLeft_;
		qreal rightBraceLeft_;
		QPointF leftBraceOffset_;
		QPointF rightBraceOffset_;
		int textSize_;

	private:
		QSize getSizeOfBrace(const QString& brace, const QFont& font, int innerHeight, QPointF* offset) const;
};

inline const QString& Braces::className() {static QString name("Braces"); return name;}

}

#endif /* BRACES_H_ */
