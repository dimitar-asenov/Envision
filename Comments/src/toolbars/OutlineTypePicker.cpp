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

#include "OutlineTypePicker.h"

namespace Comments{

OutlineTypePicker::OutlineTypePicker(QWidget *parent) : QToolButton{parent}
{
	QMenu* menu = new QMenu;
	QWidgetAction* wiAction = new QWidgetAction{this};

	QSignalMapper* signalMapper = new QSignalMapper{this};
	connect(signalMapper,  (void (QSignalMapper::*)(int)) &QSignalMapper::mapped,
			  this, &OutlineTypePicker::handleOutlineTypePicked);

	QPixmap pixmap{96, 24};
	QWidget* aWidget = new QWidget{this};
	QVBoxLayout* aLayout = new QVBoxLayout;
	aLayout->setSpacing(0);
	aWidget->setLayout(aLayout);
	wiAction->setDefaultWidget(aWidget);

	QToolButton* aButton;
	pixmap.fill(Qt::transparent);
	QPainter* aPainter = new QPainter{&pixmap};
	QPen aPen;
	aPen.setColor(Qt::black);
	aPen.setWidth(4);
	pixList_.append(QPixmap{});
	for (int i = 0; i < 6; i++)
	{
		pixmap.fill(Qt::transparent);

		aPen.setStyle(static_cast<Qt::PenStyle>(i));
		aPainter->setPen(aPen);
		aPainter->drawLine(2, 13, 94, 13);
		pixList_.append(pixmap);
		aButton = new QToolButton;
		aButton->setIconSize(QSize{96, 24});
		aButton->setStyleSheet("border: none;");
		aButton->setIcon(QIcon{pixmap});
		aLayout->addWidget(aButton);
		signalMapper->setMapping(aButton, i);
		connect(aButton,  &QToolButton::clicked, signalMapper,
				  (void (QSignalMapper::*)()) &QSignalMapper::map);
	}
	aPainter->end();

	menu->addAction(wiAction);
	this->setMenu(menu);
	this->setPopupMode(QToolButton::InstantPopup);
}

void OutlineTypePicker::handleOutlineTypePicked(int aType)
{
	this->setIcon(pixList_.at(aType));
	this->menu()->close();
	Q_EMIT outlineTypeChanged(aType);
}

void OutlineTypePicker::setselectedOutlineType(int aType)
{
	this->setIcon(pixList_.at(aType));
}

}
