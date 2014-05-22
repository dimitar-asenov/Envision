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

namespace Comments{

CommentDiagramToolbar::CommentDiagramToolbar(QWidget *parent) : QToolBar(parent)
{
	this->setWindowTitle("Diagram Editing Toolbar");

	bSelection_ = new QToolButton;
	bSelection_->setText("Selection");
	bSelection_->setCheckable(true);
	this->addWidget(bSelection_);
	bSelection_->setChecked(true);
	selection_ = true;

	this->addSeparator();

	bRectangle_ = new QToolButton;
	bRectangle_->setText("Rectangle");
	bRectangle_->setCheckable(true);
	this->addWidget(bRectangle_);
	bEllipse_ = new QToolButton;
	bEllipse_->setText("Ellipse");
	bEllipse_->setCheckable(true);
	this->addWidget(bEllipse_);
	bDiamond_ = new QToolButton;
	bDiamond_->setText("Diamond");
	bDiamond_->setCheckable(true);
	this->addWidget(bDiamond_);

	group_ = new QButtonGroup(this);
	group_->addButton(bSelection_);
	group_->addButton(bRectangle_);
	group_->addButton(bEllipse_);
	group_->addButton(bDiamond_);

	this->addSeparator();

	colorPickerBackground_ = new ColorPicker;
	this->addWidget(colorPickerBackground_);
	colorPickerBorder_ = new ColorPicker;
	this->addWidget(colorPickerBorder_);
	colorPickerText_ = new ColorPicker;
	this->addWidget(colorPickerText_);

	this->addSeparator();

	bConnections_ = new QToolButton;
	bConnections_->setText("Connections");
	bConnections_->setCheckable(true);
	this->addWidget(bConnections_);

	this->setWindowFlags(Qt::WindowStaysOnTopHint);

	connect(bSelection_, SIGNAL(toggled(bool)), this, SLOT(setSelection()));
	connect(bRectangle_, SIGNAL(toggled(bool)), this, SLOT(createRectangle()));
	connect(bEllipse_, SIGNAL(toggled(bool)), this, SLOT(createEllipse()));
	connect(bDiamond_, SIGNAL(toggled(bool)), this, SLOT(createDiamond()));
	connect(colorPickerBackground_, SIGNAL(colorChanged(QString)), this, SLOT(applyBackgroundColor(QString)));
	connect(colorPickerBorder_, SIGNAL(colorChanged(QString)), this, SLOT(applyBorderColor(QString)));
	connect(colorPickerText_, SIGNAL(colorChanged(QString)), this, SLOT(applyTextColor(QString)));
	connect(bConnections_, SIGNAL(toggled(bool)), this, SLOT(showConnectionPoints(bool)));
}

void CommentDiagramToolbar::setDiagram(VCommentDiagram *diagram)
{
	diagram_ = diagram;
	colorPickerBackground_->setColors(diagram->style()->getColors());
	colorPickerBorder_->setColors(diagram->style()->getColors());
	colorPickerText_->setColors(diagram->style()->getColors());

}


void CommentDiagramToolbar::setCurrentShape(Visualization::Item *currentShape)
{
	currentShape_ = currentShape;
	auto shape = DCast<VCommentDiagramShape>(currentShape_);

	colorPickerBackground_->setselectedColor(shape->node()->backgroundColor());
	colorPickerBorder_->setselectedColor(shape->node()->shapeColor());
	colorPickerText_->setselectedColor(shape->node()->textColor());
}

void CommentDiagramToolbar::setSelection()
{
	selection_ = true;
}

void CommentDiagramToolbar::createRectangle()
{
	//auto shape = new CommentDiagramShape(0, 0, 100, 100, CommentDiagramShape::ShapeType::Rectangle);
	//diagram_->model()->beginModification(diagram_, "create shape");
	//diagram_->shapes()->append(shape);
	//diagram_->model()->endModification();
	nextShapeToAdd_ = CommentDiagramShape::ShapeType::Rectangle;
	selection_ = false;
}

void CommentDiagramToolbar::createEllipse()
{
	//auto shape = new CommentDiagramShape(0, 0, 100, 100, CommentDiagramShape::ShapeType::Ellipse);
	//diagram_->model()->beginModification(diagram_, "create shape");
	//diagram_->shapes()->append(shape);
	//diagram_->model()->endModification();
	nextShapeToAdd_ = CommentDiagramShape::ShapeType::Ellipse;
	selection_ = false;
}

void CommentDiagramToolbar::createDiamond()
{
	//auto shape = new CommentDiagramShape(0, 0, 100, 100, CommentDiagramShape::ShapeType::Diamond);
	//diagram_->model()->beginModification(diagram_, "create shape");
	//diagram_->shapes()->append(shape);
	//diagram_->model()->endModification();
	nextShapeToAdd_ = CommentDiagramShape::ShapeType::Diamond;
	selection_ = false;
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
	selection_ = sel;
}

bool CommentDiagramToolbar::getSelectionMode()
{
	return selection_;
}

void CommentDiagramToolbar::showConnectionPoints(bool show)
{
	connection_ = true;
	diagram_->setShowConnectorPoints(show);
}

bool CommentDiagramToolbar::getConnectionMode()
{
	return connection_;
}

} /* namespace Comments */
