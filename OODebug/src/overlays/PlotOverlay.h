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

#pragma once

#include "../oodebug_api.h"
#include "PlotOverlayStyle.h"

#include "VisualizationBase/src/overlays/Overlay.h"

namespace OODebug {

class OODEBUG_API PlotOverlay : public Super<Visualization::Overlay<Visualization::Item>>
{
	ITEM_COMMON(PlotOverlay)

	public:
		enum class PlotType : int {Bars, Scatter, Array};
		PlotOverlay(Visualization::Item* associatedItem, const StyleType* style = itemStyles().get(),
						PlotType type = PlotType::Bars);
		virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

		void clear();

		void addValue(double value);
		void addValue(double xValue, double yValue);

		template <class ValueType>
		inline void updateArrayValues(const QList<ValueType>& values, const QList<int>& indices);

	protected:
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;

	private:
		QList<double> xValues_;
		QList<double> yValues_;
		double xMin_{}, xMax_{};
		double yMin_{}, yMax_{};

		QRect plotRegion_;

		std::function<void(PlotOverlay*, QPainter*)> plotFunction_{};

		void addValue(double value, QList<double>& valueList, double& minVal, double& maxVal);

		void drawXTic(QPainter* painter, const QPointF& pos, QString label);
		void drawYTic(QPainter* painter, const QPointF& pos, QString label);

		double valueRange(int dimension);
		QPointF toPlotCoordinates(QPointF position);

		void drawTics(QPainter* painter);
		void drawAxes(QPainter* painter);

		void plotBars(QPainter* painter);
		void plotScatter(QPainter* painter);
		void plotArray(QPainter* painter);
};

template <class ValueType>
void PlotOverlay::updateArrayValues(const QList<ValueType>& values, const QList<int>& indices)
{
	clear();
	for (auto val : values) yValues_ << val;
	for (int idx : indices) xValues_ << idx;

	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

} /* namespace OODebug */
