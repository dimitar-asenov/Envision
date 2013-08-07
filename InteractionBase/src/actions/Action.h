/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "../interactionbase_api.h"
#include "ModelBase/src/nodes/Node.h"
#include "VisualizationBase/src/items/Item.h"

namespace Model {
	class CompositeNode;
}

namespace Interaction {

class INTERACTIONBASE_API Action
{
	public:
		using ActionFunctionOnNode = std::function<void (Model::Node* node)>;
		using FilterFunctionOnNode = std::function<bool (Model::Node* node)>;
		using ActionFunctionOnItem = std::function<void (Visualization::Item* item)>;

		Action(const QString& shortcut, const QString& name);
		Action(const QString& shortcut, const QString& name, ActionFunctionOnNode action);
		Action(const QString& shortcut, const QString& name, ActionFunctionOnNode action, FilterFunctionOnNode filter);
		Action(const QString& shortcut, const QString& name, ActionFunctionOnItem action);
		Action(const QString& shortcut, const QString& name, ActionFunctionOnItem action, FilterFunctionOnNode filter);

		virtual ~Action();

		virtual void execute(Model::Node* node);
		virtual void execute(Visualization::Item* itemWithNode);

		const QString& shortcut() const;
		const QString& name() const;
		bool isPrefix(const QString& str) const;
		virtual bool canBeAppliedTo(Model::Node* node) const;

		static QList<Action*>& actions(int nodeTypeId);
		static QList<Action*> actions(Model::Node* node);

		template<class T> static void add(Action* action);

	private:
		QString shortcut_;
		QString name_;

		static void createStandardActionsForCompositeNode(Model::CompositeNode* node, QList<Action*>& list);
		static QString calculateSuitableShortcut(const QString& name, const QStringList& list);

		ActionFunctionOnNode actionOnNode_;
		ActionFunctionOnItem actionOnItem_;
		FilterFunctionOnNode filterOnNode_;
};

inline const QString& Action::shortcut() const { return shortcut_; }
inline const QString& Action::name() const { return name_; }
inline bool Action::isPrefix(const QString& str) const { return shortcut_.startsWith(str); }

template<class T> inline void Action::add(Action* action) { actions(T::typeIdStatic()).append(action);}

} /* namespace Interaction */
