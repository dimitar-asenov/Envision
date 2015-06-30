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
#include "MenuStyle.h"

namespace Visualization {
	class Text;
}
namespace Interaction {

/**
 * This class provides a standard interface for creating a menu which is opened
 * at the last cursor position. Any kind of node can be used and actions may be
 * associated with the nodes. The interface can also optionally contain a text field
 * which is located above the normal items, where any text can be entered.
 */
class INTERACTIONBASE_API Menu : public Super<Visualization::DeclarativeItem<Menu>> {

	ITEM_COMMON(Menu)

	public:
		virtual ~Menu();

		/**
		 * Whether this interface has a text field. Reimplement where necessary.
		 */
		virtual bool hasTextField() const;
		Visualization::Text* textField() const;
		/**
		 * The currently focused item. Either the text field,
		 * or a visualization of one of the nodes.
		 */
		Visualization::Item* focusedItem() const;
		/**
		 * Selects the given item as the focused item.
		 */
		void selectItem(Visualization::Item* item);
		/**
		 * Executes the currently focused item's function.
		 * Returns whether it was successfully executed.
		 */
		bool executeFocused();

		static void initializeForms();
		virtual int determineForm();

		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;

	protected:
		virtual bool sceneEventFilter(QGraphicsItem* watched, QEvent* event) override;
		/**
		 * The function to execute when selecting the given node.
		 */
		virtual bool onSelectNode(Model::Node* node) = 0;
		/**
		 * The function to execute when selecting the given text from the text field.
		 */
		virtual bool onSelectText(QString text) = 0;
		/**
		 * The function to hide the selection menu after successful execution
		 */
		virtual void hideSelection() = 0;

		QPoint indexOf(Model::Node* node) const;

		Menu(QVector<Model::Node*> selectableNodes, Visualization::Item* target,
							  StyleType* style = itemStyles().get(), int nrOfColumns = 3);

	private:
		QPoint correctCoordinates(QPoint point) const;
		QVector<QVector<Model::Node*>> currentNodes() const;
		QVector<QVector<Model::Node*>> arrange(QVector<Model::Node*> nodes);

		Visualization::Item* target_{};
		QPointF mousePosition_;
		QVector<QVector<Model::Node*>> currentNodes_;

		Visualization::Item* focusedItem_{};
		Visualization::Text* textField_{};

		QGraphicsEffect* selectedEffect_{};

		int nrOfColumns_{};

};

inline QVector<QVector<Model::Node*>> Menu::currentNodes() const { return currentNodes_; }
inline Visualization::Item* Menu::focusedItem() const { return focusedItem_; }
inline Visualization::Text* Menu::textField() const { return textField_; }

}
