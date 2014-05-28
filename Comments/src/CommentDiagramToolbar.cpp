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
	connect(bConnections_, SIGNAL(toggled(bool)), this, SLOT(showConnectionPoints(bool)));
	connect(aTimer_, SIGNAL(timeout()), this, SLOT(handleTimerEvent()));
}

void CommentDiagramToolbar::setDiagram(VCommentDiagram *diagram)
{
	diagram_ = diagram;
	colorPickerBackground_->setColors(diagram->style()->getColors(),diagram->style()->getColorsPerRow());
	colorPickerBorder_->setColors(diagram->style()->getColors(),diagram->style()->getColorsPerRow());
	colorPickerText_->setEnvisionTextColors();
}

void CommentDiagramToolbar::setCurrentShape(Visualization::Item *currentShape)
{
	currentShape_ = currentShape;
	auto shape = DCast<VCommentDiagramShape>(currentShape_);

	colorPickerBackground_->setselectedColor(shape->node()->backgroundColor());
	colorPickerBorder_->setselectedColor(shape->node()->shapeColor());
	colorPickerText_->setselectedColor(shape->node()->textColor());

	colorPickerBackground_->setEnabled(true);
	colorPickerBorder_->setEnabled(true);
	colorPickerText_->setEnabled(true);
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
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentShape_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setBackgroundColor(color);
	shape->node()->model()->endModification();
}

void CommentDiagramToolbar::applyBorderColor(QString color)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentShape_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setShapeColor(color);
	shape->node()->model()->endModification();
}

void CommentDiagramToolbar::applyTextColor(QString color)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentShape_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setTextColor(color);
	shape->node()->model()->endModification();
}

void CommentDiagramToolbar::setSelectionMode(bool sel)
{
	bSelection_->setChecked(sel);
}

bool CommentDiagramToolbar::getSelectionMode()
{
	return selection_;
}

void CommentDiagramToolbar::showConnectionPoints(bool show)
{
	connection_ = true;
	diagram_->setShowConnectorPoints(show);
	diagram_->scene()->scheduleUpdate();
}

bool CommentDiagramToolbar::getConnectionMode()
{
	return connection_;
}

void CommentDiagramToolbar::handleTimerEvent()
{
	//TODO
}

void CommentDiagramToolbar::show()
{
	QToolBar::show();
	aTimer_->start();
}

} /* namespace Comments */
