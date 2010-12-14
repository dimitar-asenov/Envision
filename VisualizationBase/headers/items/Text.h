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
	private:
		QStaticText text;
		TextStyle* style;
		bool isTextNew;

		qreal xOffset;
		qreal yOffset;

	public:
		Text(Item* parent, const QString& text = QString());
		Text(Item* parent, TextStyle *style, const QString& text = QString());

		void setText(const QString& newText);
		void setStyle(TextStyle *style);

		virtual bool needsUpdate();
		virtual void determineChildren();
		virtual void updateState();

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

inline void Text::setText(const QString& newText) { text.setText(newText); isTextNew = true; }
inline void Text::setStyle(TextStyle *style_)  { style = style_; isTextNew = true; };

}

#endif /* TEXT_H_ */
