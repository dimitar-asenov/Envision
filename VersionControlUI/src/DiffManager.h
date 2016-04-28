/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "versioncontrolui_api.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/util/SymbolMatcher.h"

#include "FilePersistence/src/version_control/ChangeDescription.h"


namespace FilePersistence
{
	class GitRepository;
}

namespace Visualization
{
	class ViewItem;
	class Item;
}

namespace VersionControlUI {

struct ChangeWithNodes;

class VERSIONCONTROLUI_API DiffManager
{
	public:
		DiffManager(QString oldVersion, QString newVersion, QString project, Model::SymbolMatcher contextUnitMatcher);
		void visualize();

	private:

		/**
		 * Deletes all nodes in \a container if their direct parent is also in \a container and has the same change type.
		 */
		void removeDirectChildrenOfNodesInContainer(QList<ChangeWithNodes>* container);

		/**
		 * Returns all nodes which have their direct parent present in \a nodes if the change type of the node matches
		 * the one of the parent node.
		 */
		QSet<Model::Node*> findAllNodesWithDirectParentPresent(QHash<Model::Node*, FilePersistence::ChangeType>& nodes);

		/**
		 * Searches in \a treeManager for a node with \a id and tries to get its first ancestor of type \a symbol_.
		 * Returns true if such a node was found and saved to \a resultId.
		 */
		bool findChangedNode(Model::TreeManager* treeManager, Model::NodeIdType id, Model::NodeIdType& resultId);

		/**
		 * Creates the different overlays according to the change type of the node.
		 */
		static void createOverlaysForChanges(Visualization::ViewItem* diffViewItem, QList<ChangeWithNodes> changesWithNodes);

		/**
		 * Inserts the nodes with id from \a changedNodesToVisualize into the \a diffViewItem.
		 * If a node isn't available in \a oldVersionManager or \a newVersionManager, it will add instead a text which
		 * informs about this fact.
		 */
		void visualizeChangedNodes(Model::TreeManager* oldVersionManager, QSet<Model::NodeIdType> changedNodesToVisualize,
											Model::TreeManager* newVersionManager, Visualization::ViewItem* diffViewItem);

		/**
		 * Creates a TreeManager from \a version using \a repository.
		 */
		Model::TreeManager* createTreeManagerFromVersion(FilePersistence::GitRepository& repository, QString version);

		/**
		 * Returns all items which have any of their parents present in \a items.
		 */
		static QSet<Visualization::Item*> findAllItemsWithAncestorsIn(QSet<Visualization::Item*> items);

		QString getObjectPath(Model::Node* node);

		QString oldVersion_;
		QString newVersion_;
		QString project_;
		Model::SymbolMatcher contextUnitMatcher_;


};

}
