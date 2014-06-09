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

#include "ColorPicker.h"
#include "OutlineTypePicker.h"

#include "nodes/CommentDiagram.h"
#include "VisualizationBase/src/items/Item.h"
#include "items/VComment.h"

namespace Comments {

class COMMENTS_API CommentDiagramToolbar : public QToolBar
{
		Q_OBJECT
	public:
		CommentDiagramToolbar(QWidget *parent = 0);
		void setDiagram(VCommentDiagram* diagram);
		void setCurrentShape(Visualization::Item *currentShape);
		void setCurrentConnector(Visualization::Item *currentConnector);
		void clearCurrentItem();
		void setSelectionMode(bool sel);
		bool selectionMode();
		bool connectionMode();
		void show();

		CommentDiagramShape::ShapeType nextShapeToAdd_{};

	public slots:
		void setSelection(bool sel);
		void createRectangle();
		void createEllipse();
		void createDiamond();
		void applyBackgroundColor(QString color);
		void applyBorderColor(QString color);
		void applyTextColor(QString color);
		void applyOutlineType(int i);
		void applyOutlineSize(int i);
		void showConnectionPoints(bool show);
		void handleTimerEvent();
		void applyStartArrow();
		void applyEndArrow();

	private:
		QToolButton* bSelection_{};
		QToolButton* bSelectShape_{};
		QToolButton* bConnections_{};

		ColorPicker* colorPickerBackground_{};
		ColorPicker* colorPickerBorder_{};
		ColorPicker* colorPickerText_{};

		OutlineTypePicker* OutlineTypePicker_{};
		QComboBox* cbOutlineSize_{};

		QCheckBox* boxStartArrow_{};
		QCheckBox* boxEndArrow_{};

		QButtonGroup* group_{};

		bool selection_{};
		bool connection_{};
		int colorsPerRow_{};

		VCommentDiagram* diagram_{};
		Visualization::Item* currentItem_{};

		QTimer* aTimer_{};
};

} /* namespace Comments */
