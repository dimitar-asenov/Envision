/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/
#pragma once

#include "interactionbase_api.h"
#include "ModelBase/src/nodes/Node.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.h"
#include "SelectionAtCursorItemStyle.h"

namespace Visualization {
	class Text;
}
namespace Interaction {

class INTERACTIONBASE_API SelectionAtCursorItem : public Super<Visualization::DeclarativeItem<SelectionAtCursorItem>> {

	ITEM_COMMON(SelectionAtCursorItem)

	public:
		static void show(QVector<Model::Node*> selectableNodes, Visualization::Item* target);
		static void hide();
		static bool isVisible();

		virtual ~SelectionAtCursorItem();

		Visualization::Item* focusedItem() const;
		virtual bool hasTextField() const;
		Visualization::Text* textField() const;
		void selectItem(Visualization::Item* item);
		void executeFocused();

		QPoint indexOf(Model::Node* node) const;
		QPoint correctCoordinates(QPoint point) const;
		QVector<QVector<Model::Node*>> currentNodes() const;

		static void initializeForms();
		virtual int determineForm();

		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;

	protected:
		virtual bool sceneEventFilter(QGraphicsItem* watched, QEvent* event) override;
		virtual void onSelectNode(Model::Node* node);
		virtual void onSelectText(QString text);

	private:
		static void showNow(QVector<Model::Node*> selectableNodes, Visualization::Item* target);
		static void hideNow();

		SelectionAtCursorItem(QVector<Model::Node*> selectableNodes, Visualization::Item* target,
							  StyleType* style = itemStyles().get());

		static SelectionAtCursorItem* instance;

		Visualization::Item* target_{};
		QPointF mousePosition_;
		QVector<QVector<Model::Node*>> currentNodes_;

		Visualization::Item* focusedItem_{};
		Visualization::Text* textField_{};

		QGraphicsEffect* selectedEffect_{};

		QVector<QVector<Model::Node*>> arrange(QVector<Model::Node*> nodes);
};

inline QVector<QVector<Model::Node*>> SelectionAtCursorItem::currentNodes() const { return currentNodes_; }
inline bool SelectionAtCursorItem::isVisible() { return instance; }
inline Visualization::Item* SelectionAtCursorItem::focusedItem() const { return focusedItem_; }
inline Visualization::Text* SelectionAtCursorItem::textField() const { return textField_; }

}
