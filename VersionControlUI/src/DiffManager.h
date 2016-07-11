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
#include "FilePersistence/src/version_control/Diff.h"


namespace FilePersistence
{
	class GitRepository;
	struct CommitMetaData;
}

namespace Visualization
{
	class ViewItem;
	class Item;
}

namespace VersionControlUI {

struct VersionNodes;
struct ChangeWithNodes;
struct DiffSetup;
class DiffFrame;

class VERSIONCONTROLUI_API DiffManager
{
	public:
		enum NameChangeVisualization {
				Summary = 0x1,
				NameText = 0x2,
				References = 0x4
		};
		using NameChangeVisualizations = QFlags<NameChangeVisualization>;

		DiffManager(QString project,
						QList<Model::SymbolMatcher> contextUnitMatcherPriorityList,
						Model::NodeIdType targetNodeID={},
						NameChangeVisualizations nameChangeVisualization = Summary);

		void showDiff(QString oldVersion, QString newVersion);

		void showNodeHistory(Model::NodeIdType targetNodeID, QList<QString> versions);

		void highlightChangedParts(QString oldVersion, QString newVersion, Model::TreeManager* manager);

		static void clear();

		static QString createHTMLCommitInfo(const FilePersistence::GitRepository* repository, QString revision);

		QList<DiffFrame*> computeDiffFramesAndOverlays(QString oldVersion,
																					QString newVersion, Visualization::ViewItem* viewItem);

	private:

		DiffSetup initializeDiffPrerequisites(QString oldVersion, QString newVersion);

		/**
		 * Inserts for entries of \a changes a ChangeWithNodes in \a changesWithNodes. Computes id of nodes which
		 * should be visualized and inserts them in \a changedNodesToVisualize.
		 */
		void computeDiff(QString oldVersion, QString newVersion, QList<ChangeWithNodes>& changesWithNodes,
							  QSet<Model::NodeIdType>& changedNodesToVisualize,
																 DiffSetup& diffSetup);

		/**
		 * Deletes all nodes in \a container if their direct parent is also in \a container and has the same change type.
		 */
		void removeDirectChildrenOfNodesInContainer(QList<ChangeWithNodes>& container);

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
		 * Returns a list of DiffFrame created from the ids from \a diffFrameNodeIds.
		 */
		QList<DiffFrame*> createDiffFrames(DiffSetup& diffSetup,
																			 QSet<Model::NodeIdType> diffFrameNodeIds,
																			 const QList<ChangeWithNodes>& changesWithNodes);

		/**
		 * Creates a TreeManager from \a version using \a repository.
		 */
		Model::TreeManager* createTreeManagerFromVersion(FilePersistence::GitRepository* repository, QString version);

		/**
		 * Returns all items which have an ancestor present in \a items.
		 */
		static QSet<Visualization::Item*> findAllItemsWithAncestorsIn(QSet<Visualization::Item*> items);

		/**
		 * Removes all nodes which have an ancestor present in \a container
		 */
		void removeNodesWithAncestorPresent(QSet<Model::NodeIdType>& container);

		VersionNodes retrieveVersionNodesForId(const Model::NodeIdType& id, const DiffSetup& diffSetup);

		void processNameTextChanges(FilePersistence::IdToChangeDescriptionHash& changes, const DiffSetup& diffSetup);

		static void setOverlayInformationAccordingToChangeType(FilePersistence::ChangeType changeType,
																			QString& highlightOverlayStyle, QString& highlightOverlayName,
																			QString& arrowIconOverlayStyle, QString& arrowIconOverlayName,
																			bool iconsForMoveAndModify = false);

		static Visualization::Item* addOverlaysAndReturnItem(Model::Node* node, Visualization::ViewItem* viewItem,
																QString highlightOverlayName, QString highlightOverlayStyle,
																QString arrowIconOverlayName, QString arrowIconOverlayStyle);

		bool processNameChange(Model::Node* oldNode, Model::Node* newNode, const DiffSetup& diffSetup);

		QString computeNameChangeInformation(const DiffSetup& diffSetup);

		/**
		 * Check if at least one of the nodes \a oldNode or \a newNode is in the targetNode subtree. If no targetNode is
		 * specified returns true.
		 */
		bool areInTargetNodeSubtree(Model::Node* oldNode, Model::Node* newNode,
																					const DiffSetup& diffSetup);

		/**
		 * Check if change associated with id should be visualized (e.g. if the change is part of a name change, does it
		 * satisfy the visualization constraint).
		 */
		bool shouldShowChange(Model::NodeIdType id);

		QString project_;
		QList<Model::SymbolMatcher> contextUnitMatcherPriorityList_;

		// if set specifies which node we are interested in, used for history
		Model::NodeIdType targetNodeId_;

		QHash<QString, QPair<QString, Model::NodeIdType>> nameChanges_;
		QHash<Model::NodeIdType, bool> nameChangesIdsIsNameText_;
		NameChangeVisualizations nameChangeVisualization_{Summary};

		static void scaleItems(QSet<Visualization::Item*> itemsToScale, Visualization::ViewItem* currentViewItem);

		static const QString OVERVIEW_HIGHLIGHT_OVERLAY_NAME;
		static const QString OVERVIEW_ICON_OVERLAY_NAME;

		static QHash<Visualization::ViewItem*, int> onZoomHandlerIdPerViewItem_;
		void createOverlaysForChanges(QList<ChangeWithNodes> changesWithNodes,
												Visualization::ViewItem* viewItem, DiffSetup diffSetup,
												Visualization::Item* ankerItem);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DiffManager::NameChangeVisualizations)

}
