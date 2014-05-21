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

#include "Colorchooser.h"

ColorChooser::ColorChooser(QWidget *parent) : QWidget(parent)
{
	QGridLayout *layout = new QGridLayout(this);
	colorBox_ = new QComboBox;
	colorBox_->setFixedWidth(150);
	layout->addWidget(colorBox_);
	this->setLayout(layout);

	connect(colorBox_ , SIGNAL(currentIndexChanged(int)),this,SLOT(handleIndexChanged(int)));
}

void ColorChooser::setQtStandardColors()
{
	QPixmap pixmap(100,100);
	pixmap.fill(QColor("Black"));
	colorBox_->addItem(QIcon(pixmap),"Black");
	pixmap.fill(QColor("White"));
	colorBox_->addItem(QIcon(pixmap),"White");
	pixmap.fill(QColor("Red"));
	colorBox_->addItem(QIcon(pixmap),"Red");
	pixmap.fill(QColor("Dark red"));
	colorBox_->addItem(QIcon(pixmap),"Dark red");
	pixmap.fill(QColor("Green"));
	colorBox_->addItem(QIcon(pixmap),"Green");
	pixmap.fill(QColor("Dark green"));
	colorBox_->addItem(QIcon(pixmap),"Dark green");
	pixmap.fill(QColor("Blue"));
	colorBox_->addItem(QIcon(pixmap),"Blue");
	pixmap.fill(QColor("Dark blue"));
	colorBox_->addItem(QIcon(pixmap),"Dark blue");
	pixmap.fill(QColor("Cyan"));
	colorBox_->addItem(QIcon(pixmap),"Cyan");
	pixmap.fill(QColor("Dark cyan"));
	colorBox_->addItem(QIcon(pixmap),"Dark cyan");
	pixmap.fill(QColor("Magenta"));
	colorBox_->addItem(QIcon(pixmap),"Magenta");
	pixmap.fill(QColor("Dark Magenta"));
	colorBox_->addItem(QIcon(pixmap),"Dark magenta");
	pixmap.fill(QColor("Yellow"));
	colorBox_->addItem(QIcon(pixmap),"Yellow");
	pixmap.fill(QColor("Dark Yellow"));
	colorBox_->addItem(QIcon(pixmap),"Dark yellow");
	pixmap.fill(QColor("Gray"));
	colorBox_->addItem(QIcon(pixmap),"Gray");
	pixmap.fill(QColor("Dark gray"));
	colorBox_->addItem(QIcon(pixmap),"Dark gray");
	pixmap.fill(QColor("Light gray"));
	colorBox_->addItem(QIcon(pixmap),"Light gray");
}

void ColorChooser::setEnvisionTextColors()
{
	QPixmap pixmap(100,100);
	pixmap.fill(QColor(0, 0, 0));
	colorBox_->addItem(QIcon(pixmap),"black");
	pixmap.fill(QColor(0, 0, 255));
	colorBox_->addItem(QIcon(pixmap),"blue");
	pixmap.fill(QColor(0, 255, 0));
	colorBox_->addItem(QIcon(pixmap),"green");
	pixmap.fill(QColor(191, 255, 0));
	colorBox_->addItem(QIcon(pixmap),"lime");
	pixmap.fill(QColor(255, 0, 255));
	colorBox_->addItem(QIcon(pixmap),"purple");
	pixmap.fill(QColor(255, 0, 0));
	colorBox_->addItem(QIcon(pixmap),"red");
	pixmap.fill(QColor(255, 255, 255));
	colorBox_->addItem(QIcon(pixmap),"white");
	pixmap.fill(QColor(255, 255, 0));
	colorBox_->addItem(QIcon(pixmap),"yellow");
}

void ColorChooser::setColor(QString Color)
{
	for(int i = 0; i < colorBox_->count(); i++)
	{
		if(colorBox_->itemText(i).toLower().remove(' ') == Color.toLower())
			colorBox_->setCurrentIndex(i);
	}
}

void ColorChooser::handleIndexChanged(int a)
{
	emit colorChanged(colorBox_->itemText(a).toLower().remove(' '));
}
