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

#include "CommentDiagramToolbar.h"

#include "items/VCommentDiagramShape.h"
#include "items/VCommentDiagramConnector.h"

using namespace Visualization;

namespace Comments{

CommentDiagramToolbar::CommentDiagramToolbar(QWidget *parent) : QToolBar(parent)
{
	this->setWindowTitle("Diagram Editing Toolbar");

	bSelection_ = new QToolButton;
	bSelection_->setIcon(QIcon(":/icons/selection.png"));
	bSelection_->setCheckable(true);
	this->addWidget(bSelection_);
	bSelection_->setChecked(true);
	selection_ = true;

	this->addSeparator();

	bSelectShape_ = new QToolButton;
	bSelectShape_->setIcon(QIcon(":/icons/shapes.png"));
	this->addWidget(bSelectShape_);

	QMenu* menu = new QMenu();
	menu->addAction(QIcon(":/icons/rectangle.png"),"Rectangle")->setIconVisibleInMenu(true);
	menu->addAction(QIcon(":/icons/ellipse.png"),"Ellipse")->setIconVisibleInMenu(true);
	menu->addAction(QIcon(":/icons/diamond.png"),"Diamond")->setIconVisibleInMenu(true);

	bSelectShape_->setPopupMode(QToolButton::InstantPopup);
	bSelectShape_->setMenu(menu);

	this->addSeparator();

	colorPickerBackground_ = new ColorPicker;
	colorPickerBackground_->setColorPickerType(ColorPicker::background);
	colorPickerBackground_->setEnabled(false);
	this->addWidget(colorPickerBackground_);
	colorPickerBorder_ = new ColorPicker;
	colorPickerBorder_->setColorPickerType(ColorPicker::shape);
	colorPickerBorder_->setEnabled(false);
	this->addWidget(colorPickerBorder_);
	colorPickerText_ = new ColorPicker;
	colorPickerText_->setColorPickerType(ColorPicker::text);
	colorPickerText_->setEnabled(false);
	this->addWidget(colorPickerText_);

	this->addSeparator();

	cbOutlineType_ = new QComboBox;
	cbOutlineType_->addItem("Solid Line");
	cbOutlineType_->addItem("Dash Line");
	cbOutlineType_->addItem("Dot Line");
	cbOutlineType_->addItem("Dash-Dot Line");
	cbOutlineType_->addItem("Dash-Dot-Dot Line");
	cbOutlineType_->setEnabled(false);
	this->addWidget(cbOutlineType_);

	cbOutlineSize_ = new QComboBox;
	for(int i = 1; i <= 10; i++)
	{
		cbOutlineSize_->addItem(QString::number(i));
	}
	cbOutlineSize_->setEnabled(false);
	this->addWidget(cbOutlineSize_);

	this->addSeparator();

	bConnections_ = new QToolButton;
	bConnections_->setIcon(QIcon(":/icons/connection.png"));
	bConnections_->setCheckable(true);
	this->addWidget(bConnections_);

	aTimer_ = new QTimer;
	aTimer_->setInterval(200);

	this->setWindowFlags(Qt::WindowStaysOnTopHint);

	connect(bSelection_, SIGNAL(toggled(bool)), this, SLOT(setSelection(bool)));
	connect(menu->actions().at(0), SIGNAL(triggered(bool)), this, SLOT(createRectangle()));
	connect(menu->actions().at(1), SIGNAL(triggered(bool)), this, SLOT(createEllipse()));
	connect(menu->actions().at(2), SIGNAL(triggered(bool)), this, SLOT(createDiamond()));
	connect(colorPickerBackground_, SIGNAL(colorChanged(QString)), this, SLOT(applyBackgroundColor(QString)));
	connect(colorPickerBorder_, SIGNAL(colorChanged(QString)), this, SLOT(applyBorderColor(QString)));
	connect(colorPickerText_, SIGNAL(colorChanged(QString)), this, SLOT(applyTextColor(QString)));
	connect(cbOutlineType_, SIGNAL(currentIndexChanged(int)), this, SLOT(applyOutlineType(int)));
	connect(cbOutlineSize_, SIGNAL(currentIndexChanged(int)), this, SLOT(applyOutlineSize(int)));
	connect(bConnections_, SIGNAL(toggled(bool)), this, SLOT(showConnectionPoints(bool)));
	connect(aTimer_, SIGNAL(timeout()), this, SLOT(handleTimerEvent()));
}

void CommentDiagramToolbar::setDiagram(VCommentDiagram *diagram)
{
	diagram_ = diagram;
	colorPickerBackground_->setColors(diagram->style()->getColors(),diagram->style()->getColorsPerRow());
	colorPickerBorder_->setColors(diagram->style()->getColors(),diagram->style()->getColorsPerRow());
	colorPickerText_->setEnvisionTextColors();
	bConnections_->setChecked(diagram->showConnectorPoints());
}

void CommentDiagramToolbar::setCurrentShape(Visualization::Item *currentShape)
{
	clearCurrentItem();
	currentItem_ = currentShape;
	auto shape = DCast<VCommentDiagramShape>(currentItem_);
	colorPickerBackground_->setselectedColor(shape->node()->backgroundColor());
	colorPickerBorder_->setselectedColor(shape->node()->shapeColor());
	colorPickerText_->setselectedColor(shape->node()->textColor());
	colorPickerBackground_->setEnabled(true);
	colorPickerBorder_->setEnabled(true);
	colorPickerText_->setEnabled(true);
	cbOutlineType_->setCurrentIndex(shape->node()->outlineTyp()-1);
	cbOutlineType_->setEnabled(true);
	cbOutlineSize_->setCurrentIndex(shape->node()->outlineSize()-1);
	cbOutlineSize_->setEnabled(true);
}

void CommentDiagramToolbar::setCurrentConnector(Visualization::Item *currentConnector)
{
	clearCurrentItem();
	currentItem_ = currentConnector;
	auto connector = DCast<VCommentDiagramConnector>(currentItem_);

	cbOutlineType_->setCurrentIndex(connector->node()->outlineTyp()-1);
	cbOutlineSize_->setCurrentIndex(connector->node()->outlineSize()-1);

	cbOutlineType_->setEnabled(true);
	cbOutlineSize_->setEnabled(true);
}

void CommentDiagramToolbar::clearCurrentItem()
{
	currentItem_ = nullptr;

	colorPickerBackground_->setEnabled(false);
	colorPickerBorder_->setEnabled(false);
	colorPickerText_->setEnabled(false);
	cbOutlineType_->setEnabled(false);
	cbOutlineSize_->setEnabled(false);
}

void CommentDiagramToolbar::setSelection(bool sel)
{
	selection_ = sel;
}

void CommentDiagramToolbar::createRectangle()
{
	nextShapeToAdd_ = CommentDiagramShape::ShapeType::Rectangle;
	bSelection_->setChecked(false);
}

void CommentDiagramToolbar::createEllipse()
{
	nextShapeToAdd_ = CommentDiagramShape::ShapeType::Ellipse;
	bSelection_->setChecked(false);
}

void CommentDiagramToolbar::createDiamond()
{
	nextShapeToAdd_ = CommentDiagramShape::ShapeType::Diamond;
	bSelection_->setChecked(false);
}

void CommentDiagramToolbar::applyBackgroundColor(QString color)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentItem_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setBackgroundColor(color);
	shape->node()->model()->endModification();
}

void CommentDiagramToolbar::applyBorderColor(QString color)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentItem_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setShapeColor(color);
	shape->node()->model()->endModification();
}

void CommentDiagramToolbar::applyTextColor(QString color)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentItem_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setTextColor(color);
	shape->node()->model()->endModification();
}

void CommentDiagramToolbar::applyOutlineType(int i)
{
	if(colorPickerText_->isEnabled())
	{
		auto shape = dynamic_cast<VCommentDiagramShape*>(currentItem_);
		shape->node()->model()->beginModification(shape->node(), "Setting OutlineType");
		shape->node()->setOutlineTyp(static_cast<Qt::PenStyle>(i+1));
		shape->node()->model()->endModification();
	}
	else
	{
		auto connector = dynamic_cast<VCommentDiagramConnector*>(currentItem_);
		connector->node()->model()->beginModification(connector->node(), "Setting OutlineType");
		connector->node()->setOutlineTyp(static_cast<Qt::PenStyle>(i+1));
		connector->node()->model()->endModification();
	}
}

void CommentDiagramToolbar::applyOutlineSize(int i)
{
	if(colorPickerText_->isEnabled())
	{
		auto shape = dynamic_cast<VCommentDiagramShape*>(currentItem_);
		shape->node()->model()->beginModification(shape->node(), "Setting OutlineSize");
		shape->node()->setOutlineSize(i+1);
		shape->node()->model()->endModification();
	}
	else
	{
		auto connector = dynamic_cast<VCommentDiagramConnector*>(currentItem_);
		connector->node()->model()->beginModification(connector->node(), "Setting OutlineSize");
		connector->node()->setOutlineSize(i+1);
		connector->node()->model()->endModification();
	}
}

void CommentDiagramToolbar::setSelectionMode(bool sel)
{
	bSelection_->setChecked(sel);
}

bool CommentDiagramToolbar::selectionMode()
{
	return selection_;
}

void CommentDiagramToolbar::showConnectionPoints(bool show)
{
	connection_ = true;
	diagram_->setShowConnectorPoints(show);
	diagram_->scene()->scheduleUpdate();
}

bool CommentDiagramToolbar::connectionMode()
{
	return connection_;
}

void CommentDiagramToolbar::handleTimerEvent()
{
	bool active = this->diagram_->isSelected();
	for(auto e : this->diagram_->children())
	{
		active = active || e->isSelected();
	}
	if(!active)
	{
		aTimer_->stop();
		this->close();
	}
}

void CommentDiagramToolbar::show()
{
	QToolBar::show();
	aTimer_->start();
}

} /* namespace Comments */
