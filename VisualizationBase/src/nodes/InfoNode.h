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

DECLARE_TYPED_LIST(VISUALIZATIONBASE_API, Visualization, InfoNode)

namespace Visualization {

/**
 * A StatisticsNode is the base class of all nodes which display some sort of statistics
 * on a target node. There is a register which keeps track of all these nodes.
 */
class VISUALIZATIONBASE_API InfoNode : public Super<Model::Node>
{
	DECLARE_TYPE_ID

	public:
		typedef QString (*InfoGetter) (Model::Node* target);

		InfoNode(Model::Node* target);
		InfoNode(Model::Node *target, QList<QString> enabledInfos);
		virtual ~InfoNode();

		const Model::Node* target() const;

		/**
		 * If the statistics can be represented as a string, use
		 * this to represent them as HTML text. Then the same visualization
		 * can be used for multiple statistics nodes.
		 */
		QString infoHtml() const;
		virtual void updateInfo(bool isAutoUpdate);

		void setEnabled(const QString name, bool isEnabled);
		bool isEnabled(const QString name) const;

		virtual void save(Model::PersistentStore& store) const;
		virtual void load(Model::PersistentStore& store);

		static void registerInfoGetter(const QString& name, const InfoGetter getter,
									   bool updatesAutomatically, bool enabledByDefault);

		/**
		 * Registers a new statistics node constructor.
		 * @param type The type name of the node to register.
		 * @param constructor The constructor of the node, taking a single target node.
		 */
		//static void registerStatisticsNodeType(const QString& type, const InfoNodeConstructor constructor);
		//static bool isTypeRegistered(const QString& type);
		/**
		 * Creates a new node from the registry of statistics node constructors.
		 * @param type The name of the node type to create.
		 * @param target The target of the statistics node.
		 */
		//static StatisticsNode* createNewNode(const QString& type, Model::Node* target);

	protected:
		void setInfoHtml(QString content);

	private:
		QString infoHtml_;
		QList<QString> enabledInfoGetters_;
		QHash<QString, QString> cachedInfoStrings_;
		Model::Node* target_{};

		struct InfoGetterStruct {
				InfoGetter getter_;
				bool updatesAutomatically_{};
				bool enabledByDefault_{};
				InfoGetterStruct(InfoGetter getter, bool updatesAutomatically, bool enabledByDefault)
						: getter_(getter), updatesAutomatically_(updatesAutomatically), enabledByDefault_(enabledByDefault)
				{ }
				InfoGetterStruct() {}
		};
		static QList<InfoNode*> allInfoNodes;
		static QHash<QString, InfoGetterStruct> allInfoGetters;

};

inline void InfoNode::save(Model::PersistentStore &) const { Q_ASSERT(false); }
inline void InfoNode::load(Model::PersistentStore &) { Q_ASSERT(false); }

inline QString InfoNode::infoHtml() const { return infoHtml_; }
inline void InfoNode::setInfoHtml(QString content) { infoHtml_ = content; }
inline const Model::Node* InfoNode::target() const { return target_; }

inline bool InfoNode::isEnabled(const QString name) const { return enabledInfoGetters_.contains(name); }

}