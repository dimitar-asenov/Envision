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

#pragma once

#include "ControlFlowItemStyle.h"

#include "VisualizationBase/src/items/Item.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API ControlFlowItem : public Super<Visualization::Item>
{
	ITEM_COMMON(ControlFlowItem)

	public:
		enum PreferedExitDirection {EXIT_LEFT, EXIT_RIGHT};

		ControlFlowItem(Item* parent,  const StyleType* style);

		const QPoint& entrance();
		const QPoint& exit();
		const QList< QPoint >& breaks();
		const QList< QPoint >& continues();

		void setPreferredContinueExit(PreferedExitDirection preference);
		void setPreferredBreakExit(PreferedExitDirection preference);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	protected:

		PreferedExitDirection preferredContinueExit_;
		PreferedExitDirection preferredBreakExit_;

		QPoint entrance_;
		QPoint exit_;
		QList< QPoint > breaks_;
		QList< QPoint > continues_;

		void addConnector(QList< QPoint >& points, bool arrowEnding);
		void addConnector(int xBegin, int yBegin, int xEnd, int yEnd, bool arrowEnding);
		void addConnector(const QPoint& begin, const QPoint& end, bool arrowEnding);
		void addToLastConnector(int x, int y);
		void addToLastConnector(const QPoint& point);
		void clearConnectors();

	private:
		QPainterPath connector(const QList< QPoint >& points, bool arrowEnding);

		QList< QList< QPoint > > connectors_;
		QList< bool > arrowEndings_;
};

inline const QPoint& ControlFlowItem::entrance() { return entrance_; }
inline const QPoint& ControlFlowItem::exit() { return exit_; }
inline const QList< QPoint >& ControlFlowItem::breaks() { return breaks_; }
inline const QList< QPoint >& ControlFlowItem::continues() { return continues_; }

inline void ControlFlowItem::setPreferredContinueExit(PreferedExitDirection preference)
{ preferredContinueExit_ = preference; }

inline void ControlFlowItem::setPreferredBreakExit(PreferedExitDirection preference)
{ preferredBreakExit_ = preference; }

}
