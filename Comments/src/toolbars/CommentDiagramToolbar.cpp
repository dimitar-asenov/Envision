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

#include "../items/VCommentDiagramShape.h"
#include "../items/VCommentDiagramConnector.h"

using namespace Visualization;

inline void initCommentsResources() { Q_INIT_RESOURCE(res); }

namespace Comments{

CommentDiagramToolbar::CommentDiagramToolbar(QWidget *parent) : QToolBar{parent}
{
	initCommentsResources();

	this->setIconSize(QSize{96, 24});
	this->setWindowTitle("Diagram Editing Toolbar");

	bSelection_ = new QToolButton;
	bSelection_->setIcon(QIcon{":/icons/selection.png"});
	bSelection_->setFixedSize(QSize{36, 36});
	bSelection_->setCheckable(true);
	this->addWidget(bSelection_);
	bSelection_->setChecked(true);
	selection_ = true;

	this->addSeparator();

	bSelectShape_ = new QToolButton;
	bSelectShape_->setIcon(QIcon{":/icons/shapes.png"});
	bSelectShape_->setFixedSize(QSize{42, 36});
	this->addWidget(bSelectShape_);

	QMenu* menu = new QMenu{};
	menu->addAction(QIcon{":/icons/rectangle.png"}, "Rectangle")->setIconVisibleInMenu(true);
	menu->addAction(QIcon{":/icons/ellipse.png"}, "Ellipse")->setIconVisibleInMenu(true);
	menu->addAction(QIcon{":/icons/diamond.png"}, "Diamond")->setIconVisibleInMenu(true);

	bSelectShape_->setPopupMode(QToolButton::InstantPopup);
	bSelectShape_->setMenu(menu);

	this->addSeparator();

	colorPickerBackground_ = new ColorPicker;
	colorPickerBackground_->setColorPickerType(ColorPicker::background);
	colorPickerBackground_->setFixedSize(QSize{42, 36});
	colorPickerBackground_->setEnabled(false);
	colorPickerBackground_->setColors(VCommentDiagram::itemStyles().get()->getColors(),
												 VCommentDiagram::itemStyles().get()->getColorsPerRow());
	this->addWidget(colorPickerBackground_);
	colorPickerBorder_ = new ColorPicker;
	colorPickerBorder_->setColorPickerType(ColorPicker::shape);
	colorPickerBorder_->setFixedSize(QSize{42, 36});
	colorPickerBorder_->setEnabled(false);
	colorPickerBorder_->setColors(VCommentDiagram::itemStyles().get()->getColors(),
											VCommentDiagram::itemStyles().get()->getColorsPerRow());
	this->addWidget(colorPickerBorder_);
	colorPickerText_ = new ColorPicker;
	colorPickerText_->setColorPickerType(ColorPicker::text);
	colorPickerText_->setFixedSize(QSize{42, 36});
	colorPickerText_->setEnabled(false);
	colorPickerText_->setEnvisionTextColors();
	this->addWidget(colorPickerText_);

	this->addSeparator();

	OutlineTypePicker_ = new OutlineTypePicker;
	OutlineTypePicker_->setEnabled(false);
	this->addWidget(OutlineTypePicker_);

	cbOutlineSize_ = new QComboBox;
	for (int i = 1; i <= 10; i++)
	{
		cbOutlineSize_->addItem(QString::number(i));
	}
	cbOutlineSize_->setEnabled(false);
	cbOutlineSize_->setFixedWidth(48);
	this->addWidget(cbOutlineSize_);

	this->addSeparator();

	boxStartArrow_ = new QCheckBox;
	boxStartArrow_->setIcon(QIcon{":/icons/startArrow.png"});
	boxStartArrow_->setEnabled(false);
	this->addWidget(boxStartArrow_);

	boxEndArrow_ = new QCheckBox;
	boxEndArrow_->setIcon(QIcon{":/icons/endArrow.png"});
	boxEndArrow_->setEnabled(false);
	this->addWidget(boxEndArrow_);

	bConnections_ = new QToolButton;
	bConnections_->setIcon(QIcon{":/icons/connection.png"});
	bConnections_->setFixedSize(QSize{36, 36});
	bConnections_->setCheckable(true);
	this->addWidget(bConnections_);

	aTimer_ = new QTimer;
	aTimer_->setInterval(200);

	this->setWindowFlags(Qt::WindowStaysOnTopHint);

	connect(bSelection_, &QToolButton::toggled, this, &CommentDiagramToolbar::setSelection);
	connect(menu->actions().at(0), &QAction::triggered, this, &CommentDiagramToolbar::createRectangle);
	connect(menu->actions().at(1), &QAction::triggered, this, &CommentDiagramToolbar::createEllipse);
	connect(menu->actions().at(2), &QAction::triggered, this, &CommentDiagramToolbar::createDiamond);
	connect(colorPickerBackground_, &ColorPicker::colorChanged, this, &CommentDiagramToolbar::applyBackgroundColor);
	connect(colorPickerBorder_, &ColorPicker::colorChanged, this, &CommentDiagramToolbar::applyBorderColor);
	connect(colorPickerText_, &ColorPicker::colorChanged, this, &CommentDiagramToolbar::applyTextColor);
	connect(OutlineTypePicker_, &OutlineTypePicker::outlineTypeChanged, this, &CommentDiagramToolbar::applyOutlineType);
	connect(cbOutlineSize_, (void (QComboBox::*)(int)) &QComboBox::currentIndexChanged,
			  this, &CommentDiagramToolbar::applyOutlineSize);
	connect(bConnections_, &QToolButton::toggled, this, &CommentDiagramToolbar::showConnectionPoints);
	connect(aTimer_, &QTimer::timeout, this, &CommentDiagramToolbar::handleTimerEvent);
	connect(boxStartArrow_, &QCheckBox::stateChanged, this, &CommentDiagramToolbar::applyStartArrow);
	connect(boxEndArrow_, &QCheckBox::stateChanged, this, &CommentDiagramToolbar::applyEndArrow);
}

void CommentDiagramToolbar::setDiagram(VCommentDiagram *diagram)
{
	diagram_ = diagram;
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
	OutlineTypePicker_->setselectedOutlineType(shape->node()->outlineType());
	OutlineTypePicker_->setEnabled(true);
	cbOutlineSize_->setCurrentIndex(shape->node()->outlineSize()-1);
	cbOutlineSize_->setEnabled(true);
}

void CommentDiagramToolbar::setCurrentConnector(Visualization::Item *currentConnector)
{
	clearCurrentItem();
	currentItem_ = currentConnector;
	auto connector = DCast<VCommentDiagramConnector>(currentItem_);

	OutlineTypePicker_->setselectedOutlineType(connector->node()->outlineType());
	cbOutlineSize_->setCurrentIndex(connector->node()->outlineSize()-1);

	boxStartArrow_->setChecked(connector->node()->startArrow());
	boxEndArrow_->setChecked(connector->node()->endArrow());

	OutlineTypePicker_->setEnabled(true);
	cbOutlineSize_->setEnabled(true);

	boxStartArrow_->setEnabled(true);
	boxEndArrow_->setEnabled(true);
}

void CommentDiagramToolbar::clearCurrentItem()
{
	currentItem_ = nullptr;

	colorPickerBackground_->setEnabled(false);
	colorPickerBorder_->setEnabled(false);
	colorPickerText_->setEnabled(false);
	OutlineTypePicker_->setEnabled(false);
	cbOutlineSize_->setEnabled(false);
	boxStartArrow_->setEnabled(false);
	boxEndArrow_->setEnabled(false);
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
	auto shape = DCast<VCommentDiagramShape>(currentItem_);
	shape->node()->manager()->beginModification(shape->node(), "Setting color");
	shape->node()->setBackgroundColor(color);
	shape->node()->manager()->endModification();
}

void CommentDiagramToolbar::applyBorderColor(QString color)
{
	auto shape = DCast<VCommentDiagramShape>(currentItem_);
	shape->node()->manager()->beginModification(shape->node(), "Setting color");
	shape->node()->setShapeColor(color);
	shape->node()->manager()->endModification();
}

void CommentDiagramToolbar::applyTextColor(QString color)
{
	auto shape = DCast<VCommentDiagramShape>(currentItem_);
	shape->node()->manager()->beginModification(shape->node(), "Setting color");
	shape->node()->setTextColor(color);
	shape->node()->manager()->endModification();
}

void CommentDiagramToolbar::applyOutlineType(int i)
{
	if (colorPickerText_->isEnabled())
	{
		auto shape = DCast<VCommentDiagramShape>(currentItem_);
		shape->node()->beginModification("Setting OutlineType");
		shape->node()->setOutlineType(static_cast<Qt::PenStyle>(i));
		shape->node()->endModification();
	}
	else
	{
		auto connector = DCast<VCommentDiagramConnector>(currentItem_);
		connector->node()->beginModification("Setting OutlineType");
		connector->node()->setOutlineType(static_cast<Qt::PenStyle>(i));
		connector->node()->manager()->endModification();
	}
}

void CommentDiagramToolbar::applyOutlineSize(int i)
{
	if (colorPickerText_->isEnabled())
	{
		auto shape = DCast<VCommentDiagramShape>(currentItem_);
		shape->node()->manager()->beginModification(shape->node(), "Setting OutlineSize");
		shape->node()->setOutlineSize(i+1);
		shape->node()->manager()->endModification();
	}
	else
	{
		auto connector = DCast<VCommentDiagramConnector>(currentItem_);
		connector->node()->manager()->beginModification(connector->node(), "Setting OutlineSize");
		connector->node()->setOutlineSize(i+1);
		connector->node()->manager()->endModification();
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
	if (!this->diagram_->itemOrChildHasFocus())
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

void CommentDiagramToolbar::applyStartArrow()
{
	auto connector = DCast<VCommentDiagramConnector>(currentItem_);
	connector->node()->beginModification("Setting startArrow");
	connector->node()->setStartArrow(boxStartArrow_->isChecked());
	connector->node()->endModification();
}

void CommentDiagramToolbar::applyEndArrow()
{
	auto connector = DCast<VCommentDiagramConnector>(currentItem_);
	connector->node()->beginModification("Setting endArrow");
	connector->node()->setEndArrow(boxEndArrow_->isChecked());
	connector->node()->endModification();
}

}
