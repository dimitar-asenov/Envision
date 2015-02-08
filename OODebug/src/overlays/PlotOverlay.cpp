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

PlotOverlay::PlotOverlay(Visualization::Item* associatedItem, const StyleType* style) : Super{{associatedItem}, style}{}

void OODebug::PlotOverlay::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Super::paint(painter, option, widget);

	double barWidth = 10.0;
	if (values_.size()) barWidth = style()->width() / values_.size();
	double maxHeight = *std::max_element(values_.begin(), values_.end());
	double heightScale = 1.0;
	if (maxHeight > 0.0) heightScale = style()->height() / maxHeight;

	for (int i = 0; i < values_.size(); ++i)
	{
		double scaledValue = heightScale * values_[i];
		QRectF bar(i * barWidth, getShape()->contentTop() + style()->height() - scaledValue, barWidth, scaledValue);
		painter->drawRect(bar);
		painter->fillRect(bar, QColor((i % 2 ? "red" : "black")));
	}
}

void OODebug::PlotOverlay::addValue(double value)
{
	values_ << value;
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

void PlotOverlay::determineChildren() {}

void PlotOverlay::updateGeometry(int, int)
{
	if (hasShape())
		getShape()->setInnerSize(style()->width(), style()->height());
	setPos(associatedItem()->mapToScene(associatedItem()->widthInLocal() + 10, 0));
}

} /* namespace OODebug */
