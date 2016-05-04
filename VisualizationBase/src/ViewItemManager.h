/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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

#include "visualizationbase_api.h"

namespace Model {
class TreeManager;
class Node;
}

namespace Visualization {

class Scene;
class ViewItem;
class Item;

/**
 * The ViewItemManager manages all the different ViewItems which are part
 * of a given scene. It is responsible for adding and removing views to and from
 * the scene, as well as switching between the different views.
 */
class VISUALIZATIONBASE_API ViewItemManager
{
	public:
		/**
		 * Creates a new ViewItemManager connected to the given scene.
		 */
		ViewItemManager(Scene* scene);
		~ViewItemManager();


		/**
		 * Creates a new ViewItem and adds it to the manager. The ViewItem is given the name
		 * which is supplied, and positioned at the given position, or at the next free position
		 * if the default argument (-1, -1) is used.
		 */
		ViewItem* newViewItem(const QString name = {}, QPoint position = {-1, -1});
		/**
		 * Finds the ViewItem with the given name, or nullptr if it doesn't exist.
		 */
		ViewItem* viewItem(const QString name);
		/**
		 * Switches to the given view to be the new current view. The view must
		 * already be known to the manager.
		 */
		void switchToView(ViewItem* view);
		/**
		 * Overload using the view's name. Returns whether switching was successful,
		 * i.e. whether a view with the name was found.
		 */
		bool switchToView(const QString viewName);
		/**
		 * Returns the currently visible ViewItem. Guaranteed to return a valid ViewItem.
		 */
		ViewItem* currentViewItem();
		/**
		 * Removes all existing ViewItems from the manager.
		 */
		void removeAllViewItems();
		/**
		 * Removes \a view from the manager.
		 */
		void removeViewItem(ViewItem* view);

		/**
		 * Saves the given view persistently on disk, using the given manager.
		 * Note: The view can still contain nodes handled by other managers.
		 */
		void saveView(ViewItem* view, Model::TreeManager* manager) const;
		/**
		 * Loads the given view from persistent disk storage. If it exists,
		 * it is added to the manager at the given position.
		 */
		ViewItem* loadView(QString name, QPoint position = {-1, -1});

		QVector<QVector<ViewItem*>> viewItems() const;

	private:
		friend class Item;
		friend class Scene;

		void cleanupRemovedItem(Visualization::Item* removedItem);
		void cleanupRemovedNode(Model::Node* removedNode);

		QPoint nextEmptyPosition() const;

		const QString DIRECTORY_NAME = "views";
		QString fileName(QString viewName, QString managerName) const;
		ViewItem* loadView(QString name, Model::TreeManager* manager);
		void addViewItem(ViewItem* view, QPoint position = {-1, -1});

		const int VIEW_ITEM_COLUMNS = 3;
		QVector<QVector<ViewItem*>> viewItems_;
		ViewItem* currentViewItem_{};
		Scene* scene_{};
};

inline QVector<QVector<ViewItem*>> ViewItemManager::viewItems() const { return viewItems_; }

}
