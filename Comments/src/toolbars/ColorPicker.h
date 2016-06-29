/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#pragma once

#include "../comments_api.h"

namespace Comments {
/**
 * The ColorPicker class provides a button which shows a grid of colors when clicked.
 * This colors are also clickable and can be used to select the desired color.
 */
class COMMENTS_API ColorPicker : public QToolButton
{
		Q_OBJECT
	public:
		enum ColorPickerType {background, shape, text};

		void setColors(QVector<QColor> colors, int colorsPerRow, std::function<QString (QColor)> nameForColor = {});
		/**
		 * For text Envision uses some predefined colors.
		 * This method sets the colors displayed in the grid to these specific colors.
		 */
		void setEnvisionTextColors();
		void setselectedColor(QString colorName);
		/**
		 * This sets the type of the colorpicker to \a type.
		 * The possible types are ColorPicker::background, ColorPicker::shape and ColorPicker::text.
		 * This information is then used to visualize the button of the colorpicker according to that type.
		 */
		void setColorPickerType(enum ColorPickerType type);

		void handleColorPicked(QString aColor);

	Q_SIGNALS:
		void colorChanged(QString color);

	private:
		static QList<QPair<QColor, QString>>& envisionTextColorsMap();
		ColorPickerType type_{};

};

}
