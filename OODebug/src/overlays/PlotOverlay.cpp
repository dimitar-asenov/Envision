/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "PlotOverlay.h"

#include "VisualizationBase/src/shapes/Shape.h"

namespace OODebug {

ITEM_COMMON_DEFINITIONS(PlotOverlay, "item")

PlotOverlay::PlotOverlay(Visualization::Item* associatedItem, const StyleType* style,
								 PlotType type, QStringList variableNames)
	: Super{{associatedItem}, style}
{
	setAcceptedMouseButtons(Qt::AllButtons);
	setFlags(QGraphicsItem::ItemIsMovable);
	// margins around the plot region:
	const int LEFT_MARGIN = (type == PlotType::Array ? 20 : 40);
	const int DEFAULT_MARGIN = (type == PlotType::Array ? 10 : 20);
	QSize size = {style->width() - (LEFT_MARGIN + DEFAULT_MARGIN), style->height() - 2 * DEFAULT_MARGIN};
	plotRegion_ = {{LEFT_MARGIN, DEFAULT_MARGIN}, size};

	if (PlotType::Bars == type) plotFunction_ = &PlotOverlay::plotBars;
	else if (PlotType::Scatter == type) plotFunction_ = &PlotOverlay::plotScatter;
	else if (PlotType::Array == type) plotFunction_ = &PlotOverlay::plotArray;
	else Q_ASSERT(false);

	variableNames_ = variableNames;

	setPos(associatedItem->mapToScene(associatedItem->widthInLocal() + 10, 0));
}

void OODebug::PlotOverlay::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Super::paint(painter, option, widget);

	plotFunction_(this, painter);
}

void OODebug::PlotOverlay::clear()
{
	xValues_.clear();
	yValues_.clear();
	xMin_ = 0;
	xMax_ = 0;
	yMin_ = 0;
	yMax_ = 0;
}

void OODebug::PlotOverlay::addValue(double value)
{
	addValue(value, yValues_, yMin_, yMax_);
	addValue(xValues_.size(), xValues_, xMin_, xMax_);

	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void OODebug::PlotOverlay::addValue(double xValue, double yValue)
{
	addValue(xValue, xValues_, xMin_, xMax_);
	addValue(yValue, yValues_, yMin_, yMax_);

	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void PlotOverlay::determineChildren() {}

void PlotOverlay::updateGeometry(int, int)
{
	if (hasShape())
		getShape()->setInnerSize(style()->width(), style()->height());
}

void OODebug::PlotOverlay::addValue(double value, QList<double>& valueList, double& minVal, double& maxVal)
{
	valueList << value;
	minVal = std::min(value, minVal);
	maxVal = std::max(value, maxVal);
}

void OODebug::PlotOverlay::drawXTic(QPainter* painter, const QPointF& pos, QString label)
{
	painter->drawLine(pos, {pos.x(), pos.y() + 5});
	auto fontMetrics = painter->fontMetrics();
	int height = fontMetrics.height();
	int xOffset = -(fontMetrics.averageCharWidth() * label.length()) / 2;
	painter->drawText(pos.x() + xOffset, pos.y() + height, label);
}

void OODebug::PlotOverlay::drawYTic(QPainter* painter, const QPointF& pos, QString label)
{
	painter->drawLine({pos.x() - 5, pos.y()}, pos);
	auto fontMetrics = painter->fontMetrics();
	int height = fontMetrics.height();
	int xOffset = -(fontMetrics.averageCharWidth() * (label.length() + 1));
	painter->drawText(pos.x() + xOffset, pos.y() + height / 3.0, label);
}

double OODebug::PlotOverlay::valueRange(int dimension)
{
	double min = 0, max = 0;
	if (dimension == 0)
	{
		min = xMin_;
		max = xMax_;
	}
	else
	{
		min = yMin_;
		max = yMax_;
	}
	if (max - min > 0) return max - min;
	else return 1.0; // To prevent division by zero
}

QPointF OODebug::PlotOverlay::toPlotCoordinates(QPointF position)
{
	double xRange = valueRange(0);
	double yRange = valueRange(1);
	double xScale = plotRegion_.width() / xRange;
	double yScale = plotRegion_.height() / yRange;
	return {plotRegion_.x() + position.x() * xScale, plotRegion_.bottomLeft().y() - position.y() * yScale};
}

void OODebug::PlotOverlay::drawTics(QPainter* painter)
{
	if (xValues_.size() < 2) return;
	int nTics = std::min(5, xValues_.size());
	double xStep = valueRange(0) / nTics;
	double yStep = valueRange(1) / nTics;
	// If the tics text is large adapt our plot pane:
	int maxTextLength = QString::number(yMin_).length();
	for (int i = 0; i <= nTics; ++i)
		maxTextLength = std::max(maxTextLength, QString::number(yMin_ + i * yStep).length());
	auto fontMetrics = painter->fontMetrics();
	int negativeX = plotRegion_.x() - fontMetrics.averageCharWidth() * maxTextLength;
	if (negativeX < 0) plotRegion_.setX(plotRegion_.x() - negativeX);

	for (int i = 0; i <= nTics; ++i)
	{
		double xVal = int(xMin_ + i * xStep); // We want int xtics, int xVal = .. would cause narrowing warning.
		drawXTic(painter, toPlotCoordinates({xVal, 0}), QString::number(xVal));
		double yVal = yMin_ + i * yStep;
		drawYTic(painter, toPlotCoordinates({0, yVal}), QString::number(yVal));
	}
}

void OODebug::PlotOverlay::drawAxes(QPainter* painter)
{
	painter->drawLine(plotRegion_.bottomLeft(), plotRegion_.topLeft()); // y axis
	painter->drawLine(plotRegion_.bottomLeft(), plotRegion_.bottomRight()); // x axis
}

void PlotOverlay::plotBars(QPainter* painter)
{
	drawTics(painter);
	drawAxes(painter);

	double barWidth = 1.0;
	if (xValues_.size()) barWidth = (double) plotRegion_.width() / xValues_.size();
	double heightScale = plotRegion_.height() / valueRange(1);

	for (int i = 0; i < xValues_.size(); ++i)
	{
		double scaledValue = heightScale * yValues_[i];
		auto startPoint = toPlotCoordinates({xValues_[i], yValues_[i]});
		QRectF bar(startPoint.x() - barWidth / 2.0, startPoint.y(), barWidth, scaledValue);
		painter->drawRect(bar);
		painter->fillRect(bar, QColor((i % 2 ? "red" : "black")));
	}
}

void PlotOverlay::plotScatter(QPainter* painter)
{
	drawTics(painter);
	drawAxes(painter);

	double radius = 1.0;

	QBrush brush(QColor("red"));
	painter->setBrush(brush);
	QPen pen(Qt::NoPen);
	painter->setPen(pen);
	for (int i = 0; i < xValues_.size(); ++i)
		painter->drawEllipse(toPlotCoordinates({xValues_[i], yValues_[i]}), radius, radius);
}

void OODebug::PlotOverlay::plotArray(QPainter* painter)
{
	double fieldSize = plotRegion_.width();
	if (yValues_.size())
	{
		plotRegion_.setHeight(plotRegion_.width() / yValues_.size());
		fieldSize /= yValues_.size();

		// first draw the array box:
		painter->drawLine(plotRegion_.bottomLeft(), plotRegion_.bottomRight()); // bottom line
		painter->drawLine(plotRegion_.topLeft(), plotRegion_.topRight()); // top line
		painter->drawLine(plotRegion_.bottomRight(), plotRegion_.topRight()); // right border
	}
	// draw the separators
	double plotY = plotRegion_.bottomLeft().y();
	auto fontMetrics = painter->fontMetrics();
	int fontHeight = fontMetrics.height();

	for (int i = 0; i < yValues_.size(); ++i)
	{
		QPointF pos = {plotRegion_.x() + i * fieldSize, plotY};
		painter->drawLine(QPointF(pos.x(), pos.y()), {pos.x(), pos.y() - plotRegion_.height()});

		QPointF textPos = {pos.x() + fieldSize / 3, pos.y() - plotRegion_.height() / 2 + fontHeight / 3.0};
		painter->drawText(textPos, QString::number(yValues_[i]));
	}
	// draw the pointers
	const int ARROW_HEIGHT = 30;
	for (int i = 0; i < xValues_.size(); ++i)
	{
		int index = xValues_[i];
		QPointF pos = {plotRegion_.x() + index * fieldSize, plotY + ARROW_HEIGHT};
		auto font = painter->font();
		auto newFont = font;
		newFont.setPointSize(ARROW_HEIGHT);
		newFont.setBold(true);
		painter->setFont(newFont);
		painter->drawText(pos, QString("\uA71B")); // Arrow up unicode
		painter->setFont(font);

		if (i + 1 < variableNames_.size())
			painter->drawText(QPointF(pos.x() + fieldSize / 3, pos.y()), variableNames_[i + 1]);
	}
}

} /* namespace OODebug */
