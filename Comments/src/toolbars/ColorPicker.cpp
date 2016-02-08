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

ColorPicker::ColorPicker(QWidget *parent) : QToolButton{parent}
{
	mapEnvisionTextColors_.insert("black", "#000000");
	mapEnvisionTextColors_.insert("blue", "#0000ff");
	mapEnvisionTextColors_.insert("green", "#00ff00");
	mapEnvisionTextColors_.insert("lime", "#bfff00");
	mapEnvisionTextColors_.insert("purple", "#ff00ff");
	mapEnvisionTextColors_.insert("red", "#ff0000");
	mapEnvisionTextColors_.insert("white", "#ffffff");
	mapEnvisionTextColors_.insert("yellow", "#ffff00");
}

void ColorPicker::setColors(QVector<QColor> colors, int colorsPerRow)
{
	QMenu* menu = new QMenu;
	QWidgetAction* wiAction = new QWidgetAction{this};

	QSignalMapper* signalMapper = new QSignalMapper{this};
	connect(signalMapper, (void (QSignalMapper::*)(const QString&)) &QSignalMapper::mapped,
			  this, &ColorPicker::handleColorPicked);

	QPixmap pixmap(100, 100);
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
			signalMapper->setMapping(aButton, colors.at(i).name());
			connect(aButton, &QToolButton::clicked, signalMapper,
					  (void (QSignalMapper::*)()) &QSignalMapper::map);
	}
	menu->addAction(wiAction);
	this->setMenu(menu);
	this->setPopupMode(QToolButton::InstantPopup);
}

void ColorPicker::setEnvisionTextColors()
{
	QMenu* menu = new QMenu;
	QWidgetAction* wiAction = new QWidgetAction{this};

	QSignalMapper* signalMapper = new QSignalMapper{this};
	connect(signalMapper, (void (QSignalMapper::*)(const QString&)) &QSignalMapper::mapped, this,
			  &ColorPicker::handleColorPicked);

	QPixmap pixmap(100, 100);
	QWidget* aWidget = new QWidget{this};
	QGridLayout* aLayout = new QGridLayout;
	aLayout->setSpacing(0);
	aWidget->setLayout(aLayout);
	wiAction->setDefaultWidget(aWidget);

	QToolButton* aButton;
	QMap<QString, QString>::iterator i;
	for (i = mapEnvisionTextColors_.begin(); i != mapEnvisionTextColors_.end(); ++i)
	{
		aButton = new QToolButton;
		aButton->setStyleSheet("border: none;");
		pixmap.fill(i.value());
		aButton->setIcon(QIcon{pixmap});
		aLayout->addWidget(aButton, 1, std::distance(mapEnvisionTextColors_.begin(), i));
		signalMapper->setMapping(aButton, i.key());
		connect(aButton, &QToolButton::clicked, signalMapper,
				  (void (QSignalMapper::*)()) &QSignalMapper::map);
	}

	menu->addAction(wiAction);
	this->setMenu(menu);
	this->setPopupMode(QToolButton::InstantPopup);
}

void ColorPicker::handleColorPicked(QString aColor)
{
	if (type_ == text)
		setselectedColor(mapEnvisionTextColors_.value(aColor));
	else
		setselectedColor(aColor);
	this->menu()->close();
	Q_EMIT colorChanged(aColor);
}

void ColorPicker::setselectedColor(QString aColor)
{
	QPixmap pixmap(24, 24);
	QColor selectedColor = QColor{aColor};
	pixmap.fill(Qt::transparent);
	QPainter* aPainter = new QPainter{&pixmap};
	aPainter->setPen(selectedColor);
	aPainter->setBrush(QBrush{selectedColor});
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
