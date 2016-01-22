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

#include "../VisualizationBase/src/visualizationbase_api.h"
#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/nodes/TypedList.h"
#include "ModelBase/src/nodes/nodeMacros.h"
#include "UINode.h"

namespace Visualization { class InfoNode; }
extern template class VISUALIZATIONBASE_API Model::TypedList<Visualization::InfoNode>;

namespace Visualization {

/**
 * An InfoNode is the base class of all nodes which display some sort of information
 * on a target node. There is a registry which keeps track of all possible informations
 * to be displayed.
 */
class VISUALIZATIONBASE_API InfoNode : public Super<UINode>
{
	DECLARE_TYPE_ID

	public:
		using InfoGetter = QString (*) (Model::Node* target);

		InfoNode(Model::Node* target);
		InfoNode(Model::Node *target, QJsonArray enabledInfos);
		virtual ~InfoNode();

		const Model::Node* target() const;

		/**
		 * If the infos can be represented as a string, use
		 * this to represent them as HTML text. Then the same visualization
		 * can be used for multiple statistics nodes.
		 */
		QString infoHtml() const;
		/**
		 * Updates the InfoNode fully, meaning it gets the result from all info getters.
		 */
		virtual void fullUpdate();
		/**
		 * Updates the InfoNode automatically, meaning it only updates the results from the
		 * info getters which are set to update automatically.
		 */
		virtual void automaticUpdate();

		/**
		 * Enable or disable the info getter identified by the given name.
		 */
		void setEnabled(const QString name, bool isEnabled);
		/**
		 * Move an info getter up or down in the node.
		 */
		void move(const QString& name, bool moveUp);
		/**
		 * Returns whether the info getter with the given name is enabled for this node.
		 */
		bool isEnabled(const QString name) const;

		virtual QJsonValue toJson() const override;

		/**
		 * Registers a new method to get information.
		 * @param name The name of the getter.
		 * @param getter The getter method itself, taking a node and returning a string. If this
		 *				 method is not applicable for the node supplied, it should return an empty string.
		 * @param updatesAutomatically Whether this info should always update.
		 * @param enabledByDefault Whether this info is visible by default.
		 */
		static void registerInfoGetter(const QString& name, const InfoGetter getter,
									   bool updatesAutomatically, bool enabledByDefault);

		/**
		 * Returns a list with the names of all registered info getters.
		 */
		static QStringList registeredInfoGetters();

	protected:
		void setInfoHtml(QString content);
		/**
		 * Updates the node's stored information.
		 * @param isAutoUpdate Is this an automatic update, or a forced update?
		 *					   A forced update will update everything.
		 */
		virtual void updateInfo(bool isAutoUpdate);

	private:
		QString infoHtml_;
		QList<QString> enabledInfoGetters_;
		QHash<QString, QString> cachedInfoStrings_;
		Model::Node* target_{};

		struct InfoGetterStruct {
				InfoGetter getter_{};
				bool updatesAutomatically_{};
				bool enabledByDefault_{};
		};
		static QList<InfoNode*> allInfoNodes;
		static QHash<QString, InfoGetterStruct> allInfoGetters;

};

inline QString InfoNode::infoHtml() const { return infoHtml_; }
inline const Model::Node* InfoNode::target() const { return target_; }

inline void InfoNode::fullUpdate() { updateInfo(false); }
inline void InfoNode::automaticUpdate() { updateInfo(true); }

inline bool InfoNode::isEnabled(const QString name) const { return enabledInfoGetters_.contains(name); }

inline QStringList InfoNode::registeredInfoGetters() { return allInfoGetters.keys(); }

}
