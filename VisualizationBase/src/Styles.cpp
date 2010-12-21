/***********************************************************************************************************************
 * Styles.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Styles.h"

#include <QtGui/QLinearGradient>
#include <QtGui/QRadialGradient>
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QColor>

#include <QtCore/QPointF>

namespace Visualization {

const QString Styles::STYLES_DIRECTORY_NAME = "styles";

QMutex Styles::accessMutex;
QList< QString > Styles::nodePath;
StyleNode* Styles::root;


QString Styles::getProperty(const QString& name)
{
	QStringList newPath = nodePath;
	newPath.append(name);
	return root->getProperty(newPath);
}


void Styles::load(const QString& propertyName, int& value)
{
	bool ok = true;

	value = getProperty(propertyName).toInt(&ok);
	if ( !ok ) throw VisualizationException("Could read integer value property '" + propertyName + "'");
}

void Styles::load(const QString& propertyName, bool& value)
{
	value = getProperty(propertyName).compare("true", Qt::CaseInsensitive) == 0;
}

void Styles::load(const QString& propertyName, QString& value)
{
	value = getProperty(propertyName);
}

void Styles::load(const QString& propertyName, double& value)
{
	bool ok = true;

	value = getProperty(propertyName).toDouble(&ok);
	if ( !ok ) throw VisualizationException("Could read double value property '" + propertyName + "'");
}

void Styles::loadQPointF(QPointF& value)
{
	double x, y;
	Styles::load("x", x);
	Styles::load("y", y);
	value.setX(x);
	value.setY(y);
}

void Styles::loadQColor(QColor& value)
{
	int a, r, g, b;
	Styles::load("alpha", a);
	Styles::load("red", r);
	Styles::load("green", g);
	Styles::load("blue", b);

	value = QColor(r, g, b, a);
}

void Styles::loadQPen(QPen& value)
{
	double width;
	int style, capStyle, joinStyle;
	QColor color;

	Styles::load("width", width);
	Styles::load("style", style);
	Styles::load("color", color);
	Styles::load("capStyle", capStyle);
	Styles::load("joinStyle", joinStyle);
	value = QPen(QBrush(color), width, (Qt::PenStyle) style, (Qt::PenCapStyle) capStyle, (Qt::PenJoinStyle) joinStyle);
}

void Styles::loadQFont(QFont& value)
{
	QString family;
	int size;
	bool pixelSize;
	int weight;
	int style;

	Styles::load("family", family);
	Styles::load("size", size);
	Styles::load("sizeIsInPixels", pixelSize);
	Styles::load("weight", weight);
	Styles::load("style", style);

	value = QFont();
	if ( !family.isEmpty() ) value.setFamily(family);
	value.setWeight(weight);
	if ( pixelSize ) value.setPixelSize(size);
	else value.setPointSize(size);
	value.setStyle((QFont::Style) style);
}

void Styles::loadQBrush(QBrush& value)
{
	int style;
	QColor color;

	Styles::load("style", style);

	if (style == Qt::LinearGradientPattern)
	{
		QLinearGradient gradient;
		Styles::load("gradient",gradient);
		value = QBrush(gradient);
	}
	else if (style == Qt::RadialGradientPattern)
	{
		QRadialGradient gradient;
		Styles::load("gradient",gradient);
		value = QBrush(gradient);
	}
	else
	{
		Styles::load("color", color);
		value = QBrush(color, (Qt::BrushStyle) style);
	}
}

void Styles::loadQLinearGradient(QLinearGradient& value)
{
	int coordinateMode;
	int spread;
	QPointF start;
	QPointF finish;
	QGradientStops stopPoints;

	Styles::load("start", start);
	Styles::load("finish", finish);
	Styles::load("coordinateMode", coordinateMode);
	Styles::load("spread", spread);
	Styles::load("stopPoints", stopPoints);

	value = QLinearGradient(start, finish);
	value.setSpread((QGradient::Spread) spread);
	value.setCoordinateMode((QGradient::CoordinateMode) coordinateMode);
	value.setStops(stopPoints);
}

void Styles::loadQRadialGradient(QRadialGradient& value)
{
	int coordinateMode;
	int spread;
	QPointF center;
	QPointF focal;
	double radius;
	QGradientStops stopPoints;

	Styles::load("coordinateMode", coordinateMode);
	Styles::load("spread", spread);
	Styles::load("centerPoint", center);
	Styles::load("focalPoint", focal);
	Styles::load("radius", radius);
	Styles::load("stopPoints", stopPoints);

	value = QRadialGradient(center, radius, focal);
	value.setSpread((QGradient::Spread) spread);
	value.setCoordinateMode((QGradient::CoordinateMode) coordinateMode);
	value.setStops(stopPoints);
}

}
