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


const QList<Qt::GlobalColor> PlotOverlay::PLOT_COLORS = {Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::magenta};

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
	painter->setFont(style()->font());
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
	addValues(xValues_.size(), {value});
}

void OODebug::PlotOverlay::addValue(double xValue, double yValue)
{
	addValues(xValue, {yValue});
}

void OODebug::PlotOverlay::addValues(double xValue, QList<double> yValues)
{
	addValue(xValue, xValues_, xMin_, xMax_);
	for (int i = 0; i < yValues.size(); ++i)
	{
		if (yValues_.size() <= i) yValues_ << QList<double>();
		addValue(yValues[i], yValues_[i], yMin_, yMax_);
	}
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
	painter->drawLine(pos, {pos.x(), pos.y() + style()->ticSize()});
	int xOffset = -drawnTextWidth(label) / 2; // text middle should be at tic
	painter->drawText(pos.x() + xOffset, pos.y() + drawnTextHeight(), label);
}

void OODebug::PlotOverlay::drawYTic(QPainter* painter, const QPointF& pos, QString label)
{
	painter->drawLine({pos.x() - style()->ticSize(), pos.y()}, pos);
	double xOffset = -drawnTextWidth(label) - style()->ticSize() - style()->margin();
	drawTextVerticalCenteredAt(painter, {pos.x() + xOffset, pos.y()}, label);
}

void PlotOverlay::drawTextVerticalCenteredAt(QPainter* painter, const QPointF& pos, QString label)
{
	// This is a workaround to draw text vertically aligned at position pos.
	// Drawing the text at y position: pos.y() + textHeight / 2.0 leads to wrong results.
	// The code is adapted from:
	// http://stackoverflow.com/questions/24831484/how-to-align-qpainter-drawtext-around-a-point-not-a-rectangle
	const qreal size = 32767.0;
	QPointF corner{pos.x(), pos.y() - size / 2.0};
	QRectF rect{corner, QSizeF{size, size}};
	painter->drawText(rect, Qt::AlignVCenter, label, nullptr);
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
	int maxTextLength = drawnTextWidth(QString::number(yMin_));
	for (int i = 0; i <= nTics; ++i)
		maxTextLength = std::max(maxTextLength, drawnTextWidth(QString::number(yMin_ + i * yStep)));
	int negativeX = plotRegion_.x() - maxTextLength - style()->ticSize() - 2 * style()->margin();
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
		double scaledValue = heightScale * yValues_[0][i];
		auto startPoint = toPlotCoordinates({xValues_[i], yValues_[0][i]});
		QRectF bar{startPoint.x() - barWidth / 2.0, startPoint.y(), barWidth, scaledValue};
		painter->drawRect(bar);
		painter->fillRect(bar, QColor{(i % 2 ? "red" : "black")});
	}
}

void PlotOverlay::plotScatter(QPainter* painter)
{
	drawTics(painter);
	drawAxes(painter);
	drawLegend(painter);

	const double radius = style()->scatterDotRadius();

	painter->setPen(Qt::NoPen);
	for (int i = 0; i < xValues_.size(); ++i)
	{
		for (int y = 0; y < yValues_.size(); ++y)
		{
			setBrushColor(painter, PLOT_COLORS[y < PLOT_COLORS.size() ? y : 0]);
			painter->drawEllipse(toPlotCoordinates({xValues_[i], yValues_[y][i]}), radius, radius);
		}
	}
}

void OODebug::PlotOverlay::plotArray(QPainter* painter)
{
	double fieldSize = plotRegion_.width();
	if (!yValues_.size()) yValues_ << QList<double>();
	if (yValues_[0].size())
	{
		plotRegion_.setHeight(plotRegion_.width() / yValues_[0].size());
		fieldSize /= yValues_[0].size();

		// first draw the array box:
		painter->drawPolygon(QPolygon{{plotRegion_.topLeft(), plotRegion_.topRight(),
												 plotRegion_.bottomRight(), plotRegion_.bottomLeft()}});
	}
	// draw the separators and contents
	double plotY = plotRegion_.bottomLeft().y();
	double penHalfWidth = painter->pen().width() / 2.0;
	const int fontHeight = drawnTextHeight();

	for (int i = 0; i < yValues_[0].size(); ++i)
	{
		double plotX = plotRegion_.x() + i * fieldSize;
		painter->drawLine(QPointF{plotX, plotY - penHalfWidth}, {plotX, plotY - plotRegion_.height() + penHalfWidth});
		painter->drawText(QRectF{QPointF{plotX, (qreal) plotRegion_.y()}, QSizeF{fieldSize, fieldSize}.toSize()},
								Qt::AlignVCenter | Qt::AlignHCenter, QString::number(yValues_[0][i]));
	}
	// draw the pointers into the array
	const int ARROW_HEIGHT = 8;

	// Lambda to create an isosceles triangle below an array field.
	// The bottom line is half the width of a fieldsize, center it in the field
	// the bottom line starts at + fieldSize / 4 and ends at + 3 * fieldSize / 4
	auto pointerTriangle = [=](int index) {
		double xOffset = index * fieldSize;
		double bottomWidth = fieldSize / 2;
		double padding = (fieldSize - bottomWidth) / 2;
		double topY = plotY + style()->margin();
		double bottomY = topY + ARROW_HEIGHT;
		QPointF midPoint = {plotRegion_.x() + xOffset + fieldSize / 2, topY};
		QPointF leftPoint = {plotRegion_.x() + xOffset + padding, bottomY};
		QPointF rightPoint = {plotRegion_.x() + xOffset + padding + bottomWidth, bottomY};
		return QPolygonF{{midPoint, leftPoint, rightPoint}};
	};

	setBrushColor(painter, Qt::red); // Triangle color

	QHash<int, int> valuesAt; // store how many values we have at each value for text offset
	for (int i = 0; i < xValues_.size(); ++i)
	{
		int index = xValues_[i];
		if (!valuesAt[index]++) painter->drawPolygon(pointerTriangle(index));

		if (i + 1 < variableNames_.size())
		{
			double x = plotRegion_.x() + index * fieldSize;
			// Text is below arrow and below other text. (We paint with rectangle so we have to specify top y)
			double y = plotY + ARROW_HEIGHT + 2 * style()->margin() + (valuesAt[index] - 1) * fontHeight;
			painter->drawText(QRectF{QPointF{x, y}, QSize{(int) fieldSize, fontHeight}},
									Qt::AlignVCenter | Qt::AlignHCenter, variableNames_[i + 1]);
		}
	}
}

void PlotOverlay::drawLegend(QPainter* painter)
{
	const double radius = style()->scatterDotRadius();
	Q_ASSERT(variableNames_.size() > 1); // Only draw legends when we have y dimensions.
	// first variable name is always x variable.
	int maxTextLength = drawnTextWidth(variableNames_[1]);
	for (int i = 2; i < variableNames_.size(); ++i)
		maxTextLength = std::max(maxTextLength, drawnTextWidth(variableNames_[i]));
	int legendWidth = maxTextLength + radius + 2 * style()->margin();
	int legendHeight = (variableNames_.size() - 1) * drawnTextHeight();

	const int LEGEND_MARGIN = 10;
	// Adapt plot region to make sure legend and plot do not overlap
	if (style()->width() - legendWidth - LEGEND_MARGIN < plotRegion_.right())
		plotRegion_.setRight(style()->width() - legendWidth - LEGEND_MARGIN);

	setBrushColor(painter, QColor{240, 240, 255}); // Very light gray

	QRect legendRegion{QPoint{style()->width() - legendWidth, drawnTextHeight()}, QSize{legendWidth, legendHeight}};
	painter->setPen(Qt::NoPen);
	painter->drawRect(legendRegion);

	for (int i = 0; i < variableNames_.size() - 1; ++i)
	{
		setBrushColor(painter, PLOT_COLORS[i < PLOT_COLORS.size() ? i : 0]);
		painter->setPen(Qt::NoPen); // For the dot we want no pen to have no border
		QPointF dotPosition(legendRegion.x() + style()->margin(),
								  legendRegion.y() + (i * drawnTextHeight()) + drawnTextHeight() / 2.0);
		painter->drawEllipse(dotPosition, radius, radius);
		painter->setPen(style()->pen());
		drawTextVerticalCenteredAt(painter, {dotPosition.x() + radius, dotPosition.y()}, variableNames_[i + 1]);
	}
}

void PlotOverlay::setBrushColor(QPainter* painter, QColor color)
{
	auto brush = painter->brush();
	brush.setColor(color);
	painter->setBrush(brush);
}

int PlotOverlay::drawnTextWidth(const QString& text)
{
	QFontMetrics qfm{style()->font()};
	return qfm.width(text);
}

int PlotOverlay::drawnTextHeight()
{
	QFontMetrics qfm{style()->font()};
	return qfm.height();
}

}
