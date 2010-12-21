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

#include "VisualizationException.h"

#include <QtGui/QStaticText>

namespace Visualization {

class VISUALIZATIONBASE_API Text : public Item
{
	ITEM_COMMON(Text, Item)

	private:
		QStaticText text;

		qreal xOffset;
		qreal yOffset;

	protected:
		virtual void determineChildren();
		virtual void updateState();

	public:
		Text(Item* parent, const QString& text = QString());
		Text(Item* parent, const TextStyle *style, const QString& text = QString());

		void setText(const QString& newText);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

inline void Text::setText(const QString& newText) { text.setText(newText); setUpdateNeeded(); }

}

#endif /* TEXT_H_ */
