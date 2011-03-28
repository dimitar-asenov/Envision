/***********************************************************************************************************************
 * StyleLoader.cpp
 *
 *  Created on: Mar 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "styles/StyleLoader.h"
#include "VisualizationException.h"


#include <QtGui/QLinearGradient>
#include <QtGui/QRadialGradient>
#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QColor>

namespace Visualization {

StyleLoader::StyleLoader() :
	rootStyleNode(NULL)
{
}

StyleLoader::~StyleLoader()
{
	SAFE_DELETE(rootStyleNode);
}

bool StyleLoader::hasProperty(const QString& name)
{
	QStringList newPath = nodePath;
	newPath.append(name);
	return rootStyleNode->hasProperty(newPath);
}

QString StyleLoader::getProperty(const QString& name)
{
	QStringList newPath = nodePath;
	newPath.append(name);
	return rootStyleNode->getProperty(newPath);
}

void StyleLoader::load(const QString& propertyName, int& value)
{
	bool ok = true;

	value = getProperty(propertyName).toInt(&ok);
	if ( !ok ) throw VisualizationException("Could read integer value property '" + propertyName + "'");
}

void StyleLoader::load(const QString& propertyName, bool& value)
{
	value = getProperty(propertyName).compare("true", Qt::CaseInsensitive) == 0;
}

void StyleLoader::load(const QString& propertyName, QString& value)
{
	value = getProperty(propertyName);
}

void StyleLoader::load(const QString& propertyName, double& value)
{
	bool ok = true;

	value = getProperty(propertyName).toDouble(&ok);
	if ( !ok ) throw VisualizationException("Could read double value property '" + propertyName + "'");
}

void StyleLoader::loadComposite(QPointF& value)
{
	double x, y;
	load("x", x);
	load("y", y);
	value.setX(x);
	value.setY(y);
}

void StyleLoader::loadComposite(QColor& value)
{
	int a, r, g, b;
	load("alpha", a);
	load("red", r);
	load("green", g);
	load("blue", b);

	value = QColor(r, g, b, a);
}

void StyleLoader::loadComposite(QPen& value)
{
	double width;
	int style, capStyle, joinStyle;
	QColor color;

	load("width", width);
	load("style", style);
	load("color", color);
	load("capStyle", capStyle);
	load("joinStyle", joinStyle);
	value = QPen(QBrush(color), width, (Qt::PenStyle) style, (Qt::PenCapStyle) capStyle, (Qt::PenJoinStyle) joinStyle);
}

void StyleLoader::loadComposite(QFont& value)
{
	QString family;
	int size;
	bool pixelSize;
	int weight;
	int style;
	bool underline;

	load("family", family);
	load("size", size);
	load("sizeIsInPixels", pixelSize);
	load("weight", weight);
	load("style", style);
	load("underline", underline);

	value = QFont();
	if ( !family.isEmpty() ) value.setFamily(family);
	value.setWeight(weight);
	if ( pixelSize ) value.setPixelSize(size);
	else value.setPointSize(size);
	value.setStyle((QFont::Style) style);
	value.setUnderline(underline);
}

void StyleLoader::loadComposite(QBrush& value)
{
	int style;
	QColor color;

	load("style", style);

	if (style == Qt::LinearGradientPattern)
	{
		QLinearGradient gradient;
		load("gradient",gradient);
		value = QBrush(gradient);
	}
	else if (style == Qt::RadialGradientPattern)
	{
		QRadialGradient gradient;
		load("gradient",gradient);
		value = QBrush(gradient);
	}
	else if (style != Qt::NoBrush)
	{
		load("color", color);
		value = QBrush(color, (Qt::BrushStyle) style);
	}
	else value = QBrush();
}

void StyleLoader::loadComposite(QLinearGradient& value)
{
	int coordinateMode;
	int spread;
	QPointF start;
	QPointF finish;
	QGradientStops stopPoints;

	load("start", start);
	load("finish", finish);
	load("coordinateMode", coordinateMode);
	load("spread", spread);
	load("stopPoints", stopPoints);

	value = QLinearGradient(start, finish);
	value.setSpread((QGradient::Spread) spread);
	value.setCoordinateMode((QGradient::CoordinateMode) coordinateMode);
	value.setStops(stopPoints);
}

void StyleLoader::loadComposite(QRadialGradient& value)
{
	int coordinateMode;
	int spread;
	QPointF center;
	QPointF focal;
	double radius;
	QGradientStops stopPoints;

	load("coordinateMode", coordinateMode);
	load("spread", spread);
	load("centerPoint", center);
	load("focalPoint", focal);
	load("radius", radius);
	load("stopPoints", stopPoints);

	value = QRadialGradient(center, radius, focal);
	value.setSpread((QGradient::Spread) spread);
	value.setCoordinateMode((QGradient::CoordinateMode) coordinateMode);
	value.setStops(stopPoints);
}

}
