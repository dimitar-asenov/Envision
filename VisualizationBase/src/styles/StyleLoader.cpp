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

#include "StyleLoader.h"
#include "../VisualizationException.h"

namespace Visualization {

StyleLoader::StyleLoader() :
	rootStyleNode(nullptr)
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
	if ( !ok ) throw VisualizationException{"Could not read integer value property '" + propertyName + "'"};
}

void StyleLoader::load(const QString& propertyName, bool& value)
{
	value = getProperty(propertyName).compare("true", Qt::CaseInsensitive) == 0;
}

void StyleLoader::load(const QString& propertyName, QString& value)
{
	value = getProperty(propertyName);
}


void StyleLoader::load(const QString& propertyName, QColor& value)
{
	auto colorName = getProperty(propertyName);
	value.setNamedColor(colorName);
	if (!value.isValid())
		throw VisualizationException{"Invalid color ("+ colorName +") of property '" + propertyName + "'"};
}

void StyleLoader::load(const QString& propertyName, double& value)
{
	bool ok = true;

	value = getProperty(propertyName).toDouble(&ok);
	if ( !ok ) throw VisualizationException{"Could not read double value property '" + propertyName + "'"};
}

void StyleLoader::loadComposite(QPointF& value)
{
	double x, y;
	load("x", x);
	load("y", y);
	value.setX(x);
	value.setY(y);
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
	value = QPen{QBrush{color}, width, (Qt::PenStyle) style, (Qt::PenCapStyle) capStyle, (Qt::PenJoinStyle) joinStyle};
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

	value = QFont{};
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
		load("gradient", gradient);
		value = QBrush{gradient};
	}
	else if (style == Qt::RadialGradientPattern)
	{
		QRadialGradient gradient;
		load("gradient", gradient);
		value = QBrush{gradient};
	}
	else if (style != Qt::NoBrush)
	{
		load("color", color);
		value = QBrush{color, (Qt::BrushStyle) style};
	}
	else value = QBrush{};
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

	value = QLinearGradient{start, finish};
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

	value = QRadialGradient{center, radius, focal};
	value.setSpread((QGradient::Spread) spread);
	value.setCoordinateMode((QGradient::CoordinateMode) coordinateMode);
	value.setStops(stopPoints);
}

}
