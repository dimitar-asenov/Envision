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

#include "ColorPicker.h"

namespace Comments{

QList<QPair<QColor, QString>>& ColorPicker::envisionTextColorsMap()
{
	static QList<QPair<QColor, QString>> map
	{
		{QColor{"#000000"}, "Black"},
		{QColor{"#434343"}, "DarkGray3"},
		{QColor{"#666666"}, "DarkGray2"},
		{QColor{"#999999"}, "DarkGray1"},
		{QColor{"#b7b7b7"}, "DarkGray0"},
		{QColor{"#cccccc"}, "Gray"},
		{QColor{"#d9d9d9"}, "LightGray"},
		{QColor{"#efefef"}, "LightGray1"},
		{QColor{"#f3f3f3"}, "LightGray2"},
		{QColor{"#ffffff"}, "White"},

		{QColor{"#980000"}, "RedBerry"},
		{QColor{"#ff0000"}, "Red"},
		{QColor{"#ff9900"}, "Orange"},
		{QColor{"#ffff00"}, "Yellow"},
		{QColor{"#00ff00"}, "Green"},
		{QColor{"#00ffff"}, "Cyan"},
		{QColor{"#4a86e8"}, "CornflowerBlue"},
		{QColor{"#0000ff"}, "Blue"},
		{QColor{"#9900ff"}, "Purple"},
		{QColor{"#ff00ff"}, "Magenta"},

		{QColor{"#e6b8af"}, "LightRedBerry3"},
		{QColor{"#f4cccc"}, "LightRed3"},
		{QColor{"#fce5cd"}, "LightOrange3"},
		{QColor{"#fff2cc"}, "LightYellow3"},
		{QColor{"#d9ead3"}, "LightGreen3"},
		{QColor{"#d0e0e3"}, "LightCyan3"},
		{QColor{"#c9daf8"}, "LightCornflowerBlue3"},
		{QColor{"#cfe2f3"}, "LightBlue3"},
		{QColor{"#d9d2e9"}, "LightPurple3"},
		{QColor{"#ead1dc"}, "LightMagenta3"},

		{QColor{"#db7e6b"}, "LightRedBerry2"},
		{QColor{"#e89898"}, "LightRed2"},
		{QColor{"#f7c99b"}, "LightOrange2"},
		{QColor{"#fde398"}, "LightYellow2"},
		{QColor{"#b5d5a7"}, "LightGreen2"},
		{QColor{"#a1c2c7"}, "LightCyan2"},
		{QColor{"#a3c0f2"}, "LightCornflowerBlue2"},
		{QColor{"#9ec3e6"}, "LightBlue2"},
		{QColor{"#b3a6d4"}, "LightPurple2"},
		{QColor{"#d3a5bc"}, "LightMagenta2"},

		{QColor{"#ca4126"}, "LightRedBerry1"},
		{QColor{"#de6666"}, "LightRed1"},
		{QColor{"#f4b16b"}, "LightOrange1"},
		{QColor{"#fdd766"}, "LightYellow1"},
		{QColor{"#92c27d"}, "LightGreen1"},
		{QColor{"#76a4ae"}, "LightCyan1"},
		{QColor{"#6d9de9"}, "LightCornflowerBlue1"},
		{QColor{"#6fa7da"}, "LightBlue1"},
		{QColor{"#8d7cc1"}, "LightPurple1"},
		{QColor{"#c07b9f"}, "LightMagenta1"},

		{QColor{"#a51d02"}, "DarkRedBerry1"},
		{QColor{"#ca0202"}, "DarkRed1"},
		{QColor{"#e49039"}, "DarkOrange1"},
		{QColor{"#efc033"}, "DarkYellow1"},
		{QColor{"#6aa74f"}, "DarkGreen1"},
		{QColor{"#45808d"}, "DarkCyan1"},
		{QColor{"#3d78d6"}, "DarkCornflowerBlue1"},
		{QColor{"#3e84c4"}, "DarkBlue1"},
		{QColor{"#674ea6"}, "DarkPurple1"},
		{QColor{"#a54d79"}, "DarkMagenta1"},

		{QColor{"#85200c"}, "DarkRedBerry2"},
		{QColor{"#990000"}, "DarkRed2"},
		{QColor{"#b45f06"}, "DarkOrange2"},
		{QColor{"#bf9000"}, "DarkYellow2"},
		{QColor{"#38761d"}, "DarkGreen2"},
		{QColor{"#134f5c"}, "DarkCyan2"},
		{QColor{"#1155cc"}, "DarkCornflowerBlue2"},
		{QColor{"#0b5394"}, "DarkBlue2"},
		{QColor{"#351c75"}, "DarkPurple2"},
		{QColor{"#741b47"}, "DarkMagenta2"},

		{QColor{"#5b0f00"}, "DarkRedBerry3"},
		{QColor{"#660000"}, "DarkRed3"},
		{QColor{"#783f04"}, "DarkOrange3"},
		{QColor{"#7f6000"}, "DarkYellow3"},
		{QColor{"#274e13"}, "DarkGreen3"},
		{QColor{"#0c343d"}, "DarkCyan3"},
		{QColor{"#1c4587"}, "DarkCornflowerBlue3"},
		{QColor{"#073763"}, "DarkBlue3"},
		{QColor{"#20124d"}, "DarkPurple3"},
		{QColor{"#4c1130"}, "DarkMagenta3"}
	};

	return map;
}

void ColorPicker::setColors(QVector<QColor> colors, int colorsPerRow, std::function<QString (QColor)> nameForColor)
{
	QMenu* menu = new QMenu;
	QWidgetAction* wiAction = new QWidgetAction{this};

	QSignalMapper* signalMapper = new QSignalMapper{this};
	connect(signalMapper, (void (QSignalMapper::*)(const QString&)) &QSignalMapper::mappedString,
			  this, &ColorPicker::handleColorPicked);

	QPixmap pixmap{100, 100};
	QWidget* aWidget = new QWidget{this};
	QGridLayout* aLayout = new QGridLayout;
	aLayout->setSpacing(0);
	aWidget->setLayout(aLayout);
	wiAction->setDefaultWidget(aWidget);

	QToolButton* aButton;
	for (int i = 0; i < colors.size(); i++)
	{
			aButton = new QToolButton;
			aButton->setStyleSheet("border: none;");
			pixmap.fill(colors.at(i));
			aButton->setIcon(QIcon{pixmap});
			aLayout->addWidget(aButton, i/colorsPerRow, i%colorsPerRow);
			signalMapper->setMapping(aButton, (nameForColor?nameForColor(colors.at(i)) : colors.at(i).name()) );
			connect(aButton, &QToolButton::clicked, signalMapper,
					  (void (QSignalMapper::*)()) &QSignalMapper::map);
	}
	menu->addAction(wiAction);
	this->setMenu(menu);
	this->setPopupMode(QToolButton::InstantPopup);
}

void ColorPicker::setEnvisionTextColors()
{
	QVector<QColor> colors;
	for (auto pair : envisionTextColorsMap())
		colors << pair.first;

	setColors(colors, 10, [](QColor color) {
		for (auto pair : envisionTextColorsMap())
			if (pair.first == color)
				return "colors/" + pair.second;

		Q_ASSERT(false);
	});
}

void ColorPicker::handleColorPicked(QString aColor)
{
	setselectedColor(aColor);
	this->menu()->close();
	Q_EMIT colorChanged(aColor);
}

void ColorPicker::setselectedColor(QString colorName)
{
	QColor color{colorName};
	for (auto envisionColor : envisionTextColorsMap())
		if (envisionColor.second == colorName.mid(QString{"colors/"}.length()))
		{
			color = envisionColor.first;
			break;
		}

	QPixmap pixmap{24, 24};
	pixmap.fill(Qt::transparent);
	QPainter* aPainter = new QPainter{&pixmap};
	aPainter->setPen(color);
	aPainter->setBrush(color);
	aPainter->drawRect(12, 12, 12, 12);

	aPainter->setPen(Qt::black);
	QFont font = aPainter->font();
	font.setPointSize(16);
	aPainter->setFont(font);
	switch (type_)
	{
		case background:
			aPainter->setBrush(QBrush{Qt::gray});
			aPainter->drawRect(5, 5, 13, 13);
		break;
		case shape:
			aPainter->setBrush(QBrush{Qt::transparent});
			aPainter->drawRect(5, 5, 13, 13);
		break;
		case text:
			aPainter->drawText(3, 20, "T");
		break;
	}
	aPainter->end();
	this->setIcon(QIcon{pixmap});
}

void ColorPicker::setColorPickerType(ColorPickerType type)
{
	type_ = type;
}

}
