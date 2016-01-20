/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "Action.h"

#include "../InteractionBaseException.h"
#include "../events/SetCursorEvent.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/List.h"

using namespace Model;
using namespace Visualization;

namespace Interaction {

QList<Action*>& Action::actions(int nodeTypeId)
{
	static QMap<int, QList<Action*>* > map;
	auto val = map.find(nodeTypeId);
	if (val != map.end()) return **val;
	else
	{
		map.insert(nodeTypeId, new QList<Action*>{});
		return *map.value(nodeTypeId);
	}
}

QList<Action*> Action::actions(Node* node)
{
	auto & completeList = actions(node->typeId());

	if (completeList.isEmpty())
	{
		if (auto cn = DCast<CompositeNode>(node))
			createStandardActionsForCompositeNode(cn, completeList);

		createStandardRemoveAction(completeList);
	}

	QList<Action*> filteredList;
	for (auto a : completeList)
		if (a->canBeAppliedTo(node)) filteredList.append(a);

	return filteredList;
}

Action::Action(const QString& shortcut, const QString& name)
	: shortcut_{shortcut}, name_{name}
{}

Action::Action(const QString& shortcut, const QString& name, ActionFunctionOnNode action)
	: shortcut_{shortcut}, name_{name}, actionOnNode_{action}
{}


Action::Action(const QString& shortcut, const QString& name, ActionFunctionOnNode action, FilterFunctionOnNode filter)
: shortcut_{shortcut}, name_{name}, actionOnNode_{action}, filterOnNode_{filter}
{}

Action::Action(const QString& shortcut, const QString& name, ActionFunctionOnItem action)
: shortcut_{shortcut}, name_{name}, actionOnItem_{action}
{}

Action::Action(const QString& shortcut, const QString& name, ActionFunctionOnItem action, FilterFunctionOnNode filter)
: shortcut_{shortcut}, name_{name}, actionOnItem_{action}, filterOnNode_{filter}
{}

Action::~Action()
{
}

void Action::execute(Node* node)
{
	if (actionOnNode_) actionOnNode_(node);
	else throw InteractionBaseException("Nothing to execute for action" + name_ + "(" + shortcut_ + ").");
}

void Action::execute(Visualization::Item* itemWithNode)
{
	Q_ASSERT(itemWithNode->node());
	if (actionOnItem_) actionOnItem_(itemWithNode);
	else execute(itemWithNode->node());
}

bool Action::canBeAppliedTo(Model::Node* node) const
{
	if (filterOnNode_) return filterOnNode_(node);
	else return true;
}

void Action::createStandardActionsForCompositeNode(CompositeNode* node, QList<Action*>& list)
{
	auto meta = node->meta();

	QStringList shortcuts;

	for (int level = meta.numLevels() - 1; level >= 0 ; --level)
	{
		AttributeChain* currentLevel = meta.level(level);

		for (int i = 0; i < currentLevel->size(); ++i)
		{
			QString name = (*currentLevel)[i].name();
			if (name.startsWith("_")) continue; // Avoid "hidden" attributes which should only be modified indirectly

			bool isOptional = (*currentLevel)[i].optional();
			bool isList = (*currentLevel)[i].type().startsWith("TypedListOf")
					|| (*currentLevel)[i].type().endsWith("List");

			if ( isOptional )
			{
				// Add create optional, if it is a list, also create an element
				shortcuts << calculateSuitableShortcut(name, shortcuts);
				list.append( new Action{shortcuts.last(), "+ " + name,
						Action::ActionFunctionOnItem([name, isList](Item* item){
							auto cn = static_cast<CompositeNode*>(item->node());
							Q_ASSERT( cn->get(name) == nullptr);

							cn->beginModification("add " + name);
							cn->setDefault(name);
							Node* nodeToSelect = cn->get(name);
							if (isList)
							{
								auto newList = DCast<List>(cn->get(name));
								nodeToSelect = newList->createDefaultElement();
								newList->append(nodeToSelect);

							}
							cn->endModification();

							item->setUpdateNeededForChildItem(Item::StandardUpdate, nodeToSelect);
							item->scene()->addPostEventAction(new SetCursorEvent{item, nodeToSelect});
						}),
						[name](Node* node){
							auto cn = DCast<CompositeNode>(node);
							return cn && cn->get(name) == nullptr;
						}
					}
				);
			}
			else if ( isList )
			{
				// Add create list entry
				shortcuts << calculateSuitableShortcut(name, shortcuts);
				list.append( new Action{shortcuts.last(), "+ " + name,
						Action::ActionFunctionOnItem([name](Item* item){
							auto cn = static_cast<CompositeNode*>(item->node());
							auto listInNode = DCast<List>(cn->get(name));
							Q_ASSERT(listInNode);
							Node* nodeToSelect = nullptr;

							cn->beginModification("add " + name + " entry");
							nodeToSelect = listInNode->createDefaultElement();
							listInNode->append(nodeToSelect);
							cn->endModification();

							item->setUpdateNeededForChildItem(Item::StandardUpdate, nodeToSelect);
							item->scene()->addPostEventAction(new SetCursorEvent{item, nodeToSelect});
						}),
						[name](Node* node){
							auto cn = DCast<CompositeNode>(node);
							return cn && cn->get(name) != nullptr;
						}
					}
				);
			}
		}
	}
}

void Action::createStandardRemoveAction(QList<Action*>& list)
{
	// Remove optional element or remove an element from a list or reset a mandatory element to its default value.
	list.append( new Action{"remove", "",
		Action::ActionFunctionOnItem([](Item* item){

			item->setUpdateNeeded(Item::StandardUpdate); // Request an update before deleting any nodes.

			auto node = item->node();
			auto parent = node->parent();
			auto scene = item->scene();
			if (auto listParent = DCast<List>(parent))
			{
				listParent->beginModification("remove node");
				listParent->remove(node);
				listParent->endModification();
			}
			else if (auto cnParent = DCast<CompositeNode>(parent))
			{
				cnParent->beginModification("remove node");
				cnParent->remove(node);
				cnParent->endModification();
			}

			scene->addPostEventAction(new SetCursorEvent{scene, parent});
		}),
		[](Node* node){
			return DCast<CompositeNode>(node->parent()) || DCast<List>(node->parent());
		}}
	);
}

QString Action::calculateSuitableShortcut(const QString& name, const QStringList& list)
{
	// Try to find a shortcut with as few letters as possible
	QString nameWithSpace = name + " ";
	for (int i = 1; i <= nameWithSpace.length(); ++i)
	{
		QString prefix = nameWithSpace.left(i);
		bool found = false;
		for (auto s : list)
			if (s.startsWith(prefix, Qt::CaseInsensitive))
			{
				found = true;
				break;
			}

		if (!found) return prefix;
	}

	// We find something
	Q_ASSERT(false);
	return QString();
}

}
