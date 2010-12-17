/***********************************************************************************************************************
 * Text.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXT_H_
#define TEXT_H_

#include "visualizationbase_api.h"

#include "TextStyle.h"
#include "../Item.h"

#include <QtGui/QStaticText>

namespace Visualization {

class VISUALIZATIONBASE_API Text : public Item
{
	public:
		typedef TextStyle StyleType;

	private:
		QStaticText text;
		TextStyle* style;

		qreal xOffset;
		qreal yOffset;

	protected:
		virtual void determineChildren();
		virtual void updateState();

	public:
		Text(Item* parent, const QString& text = QString());
		Text(Item* parent, TextStyle *style, const QString& text = QString());
		static const QString& className();

		void setText(const QString& newText);
		void setStyle(TextStyle *style);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

inline const QString& Text::className() {static QString name("Text"); return name;}
inline void Text::setText(const QString& newText) { text.setText(newText); setUpdateNeeded(); }
inline void Text::setStyle(TextStyle *style_)  { style = style_; setUpdateNeeded(); };

}

#endif /* TEXT_H_ */
