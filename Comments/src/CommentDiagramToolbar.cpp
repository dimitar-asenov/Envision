#include "CommentDiagramToolbar.h"

#include "items/VCommentDiagramShape.h"

namespace Comments{

CommentDiagramToolbar::CommentDiagramToolbar(QWidget *parent) : QToolBar(parent)
{
	bRectangle_ = new QToolButton;
	bRectangle_->setText("Rectangle");
	this->addWidget(bRectangle_);
	bEllipse_ = new QToolButton;
	bEllipse_->setText("Ellipse");
	this->addWidget(bEllipse_);
	bDiamond_ = new QToolButton;
	bDiamond_->setText("Diamond");
	this->addWidget(bDiamond_);

	this->addSeparator();

	colorPickerBackground_ = new QtColorPicker(0,-1,false);
	colorPickerBackground_->setStandardColors();
	colorPickerBackground_->setFixedWidth(125);
	this->addWidget(colorPickerBackground_);
	colorPickerBorder_ = new QtColorPicker(0,-1,false);
	colorPickerBorder_->setStandardColors();
	colorPickerBorder_->setFixedWidth(125);
	this->addWidget(colorPickerBorder_);
	colorPickerText_ = new QtColorPicker(0,-1,false);
	colorPickerText_->insertColor(QColor(0, 0, 0), "black");
	colorPickerText_->insertColor(QColor(0, 0, 255), "blue");
	colorPickerText_->insertColor(QColor(0, 255, 0), "green");
	colorPickerText_->insertColor(QColor(191, 255, 0), "lime");
	colorPickerText_->insertColor(QColor(255, 0, 255), "purple");
	colorPickerText_->insertColor(QColor(255, 0, 0), "red");
	colorPickerText_->insertColor(QColor(255, 255, 255), "white");
	colorPickerText_->insertColor(QColor(255, 255, 0), "yellow");
	colorPickerText_->setFixedWidth(125);
	this->addWidget(colorPickerText_);

	mapQtColors_.insert("white", Qt::white);
	mapQtColors_.insert("black", Qt::black);
	mapQtColors_.insert("red", Qt::red);
	mapQtColors_.insert("darkred", Qt::darkRed);
	mapQtColors_.insert("green", Qt::green);
	mapQtColors_.insert("darkgreen", Qt::darkGreen);
	mapQtColors_.insert("blue", Qt::blue);
	mapQtColors_.insert("darkblue", Qt::darkBlue);
	mapQtColors_.insert("cyan", Qt::cyan);
	mapQtColors_.insert("darkcyan", Qt::darkCyan);
	mapQtColors_.insert("magenta", Qt::magenta);
	mapQtColors_.insert("darkmagenta", Qt::darkMagenta);
	mapQtColors_.insert("yellow", Qt::yellow);
	mapQtColors_.insert("darkyellow", Qt::darkYellow);
	mapQtColors_.insert("gray", Qt::gray);
	mapQtColors_.insert("darkgray", Qt::darkGray);
	mapQtColors_.insert("lightgray", Qt::lightGray);

	mapTextColors_.insert("black",QColor(0,0,0));
	mapTextColors_.insert("blue",QColor(0, 0, 255));
	mapTextColors_.insert("green",QColor(0, 255, 0));
	mapTextColors_.insert("lime",QColor(191, 255, 0));
	mapTextColors_.insert("purple",QColor(255, 0, 255));
	mapTextColors_.insert("red",QColor(255, 0, 0));
	mapTextColors_.insert("white",QColor(255, 255, 255));
	mapTextColors_.insert("yellow",QColor(255, 255, 0));

	this->setWindowFlags(Qt::WindowStaysOnTopHint);

	connect(bRectangle_, SIGNAL(clicked()), this, SLOT(createRectangle()));
	connect(bEllipse_, SIGNAL(clicked()), this, SLOT(createEllipse()));
	connect(bDiamond_, SIGNAL(clicked()), this, SLOT(createDiamond()));
	connect(colorPickerBackground_, SIGNAL(colorChanged(const QColor)), this, SLOT(applyBackgroundColor()));
	connect(colorPickerBorder_, SIGNAL(colorChanged(const QColor)), this, SLOT(applyBorderColor()));
	connect(colorPickerText_, SIGNAL(colorChanged(const QColor)), this, SLOT(applyTextColor()));
}

void CommentDiagramToolbar::setCurrentShape(Visualization::Item *currentShape)
{
	currentShape_ = currentShape;
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentShape_);
	colorPickerBackground_->setCurrentColor(QColor(mapQtColors_.value(shape->node()->backgroundColor().toLower())));

	colorPickerBorder_->setCurrentColor(QColor(mapQtColors_.value(shape->node()->shapeColor().toLower())));

	colorPickerText_->setCurrentColor(mapTextColors_.value(shape->node()->textColor()));
}

void CommentDiagramToolbar::createRectangle()
{
	auto shape = new CommentDiagramShape(0, 0, 100, 100, CommentDiagramShape::ShapeType::Rectangle);
	diagram_->model()->beginModification(diagram_, "create shape");
	diagram_->shapes()->append(shape);
	diagram_->model()->endModification();
}

void CommentDiagramToolbar::createEllipse()
{
	auto shape = new CommentDiagramShape(0, 0, 100, 100, CommentDiagramShape::ShapeType::Ellipse);
	diagram_->model()->beginModification(diagram_, "create shape");
	diagram_->shapes()->append(shape);
	diagram_->model()->endModification();
}

void CommentDiagramToolbar::createDiamond()
{
	auto shape = new CommentDiagramShape(0, 0, 100, 100, CommentDiagramShape::ShapeType::Diamond);
	diagram_->model()->beginModification(diagram_, "create shape");
	diagram_->shapes()->append(shape);
	diagram_->model()->endModification();
}

void CommentDiagramToolbar::applyBackgroundColor()
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentShape_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setBackgroundColor(colorPickerBackground_->text().remove(' '));
	shape->node()->model()->endModification();
}

void CommentDiagramToolbar::applyBorderColor()
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentShape_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setShapeColor(colorPickerBorder_->text().remove(' '));
	shape->node()->model()->endModification();
}

void CommentDiagramToolbar::applyTextColor()
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(currentShape_);
	shape->node()->model()->beginModification(shape->node(), "Setting color");
	shape->node()->setTextColor(colorPickerText_->text().remove(' '));
	shape->node()->model()->endModification();
}

} /* namespace Comments */
