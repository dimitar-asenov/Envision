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

#include "StringOffsetProvider.h"
#include "StringComponents.h"
#include "GridBasedOffsetProvider.h"
#include "TextRendererStringOffsetProvider.h"

#include "VisualizationBase/src/cursor/LayoutCursor.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/TextRenderer.h"
#include "Core/src/AdapterManager.h"
#include "VisualizationBase/src/items/LayoutProvider.h"
#include "VisualizationBase/src/declarative/DeclarativeItemBase.h"
#include "VisualizationBase/src/declarative/GridLayoutFormElement.h"

namespace OOInteraction {

QList<StringOffsetProvider::AllowGridBasedProviderFunction> StringOffsetProvider::allowGridBasedProviderFunctions_;
void StringOffsetProvider::allowGridBasedProvider(AllowGridBasedProviderFunction allow)
{
	allowGridBasedProviderFunctions_.append(allow);
}

StringOffsetProvider::StringOffsetProvider(Visualization::Item* item) : vis_{item}
{
}

StringOffsetProvider::~StringOffsetProvider()
{
}

QString StringOffsetProvider::string()
{
	QString res = components().join("");
	return res.isNull() ? QString{""} : res;
}

QStringList StringOffsetProvider::components()
{
	if (!vis_) return QStringList{};
	return components(vis_->node());
}

QStringList StringOffsetProvider::components(Model::Node* node)
{
	if (!node) return QStringList{};
	else return StringComponents{node}.components();
}

QString StringOffsetProvider::stringFromComponenets(Model::Node* node)
{
	if (!node) return QString{};

	QString res = components(node).join("");
	return res.isNull() ? QString{""} : res;
}

QString StringOffsetProvider::stringFromComponenets(Visualization::Item* item)
{
	if (!item) return QString{};
	return stringFromComponenets(item->node());
}

QString StringOffsetProvider::stringFromStringOffsetProvider(Visualization::Item* item)
{
	if (!item) return QString{};

	QString result;
	StringOffsetProvider* sp = Core::AdapterManager::adapt<StringOffsetProvider>(item);
	if (sp)
	{
		result = sp->string();
		SAFE_DELETE(sp);
	}

	return result;
}

bool StringOffsetProvider::isIndivisible()
{
	return false;
}

bool StringOffsetProvider::setOffsetInItem(int offset, Visualization::Item* item)
{
	if (!item) return false;
	auto child = Core::AdapterManager::adapt<StringOffsetProvider>(item);
	if (child)
	{
		if (offset > 0 && child->isIndivisible()) child->setOffset(child->string().length());
		else child->setOffset(offset);
		SAFE_DELETE(child);
		return true;
	}
	return false;
}

int StringOffsetProvider::itemOffset(Visualization::Item* item, int stringComponentLenght, Qt::Key key)
{
	StringOffsetProvider* child = Core::AdapterManager::adapt<StringOffsetProvider>(item);
	Q_ASSERT(child);

	int offset = child->offset(key);
	if (offset > 0 && child->isIndivisible()) offset = stringComponentLenght;
	SAFE_DELETE(child);

	return offset;
}

bool StringOffsetProvider::setOffsetInListItem(int& offset, Visualization::VList* list,
		const QString& prefix, const QString& separator, const QString& /*postfix*/)
{
	if (offset == 0) return list->moveCursor( Visualization::Item::MoveOnTopLeft);

	offset -= prefix.size();
	QStringList components = StringOffsetProvider::components(list->node());

	for (int i = 0; i<components.size(); ++i)
	{
		if (i>0) offset -= separator.size();

		if (offset <= components[i].size())
		{
			// If the offset is in a node before any visualized node go to the beginning of the list.
			if (i < list->rangeBegin() )
				return list->moveCursor( Visualization::Item::MoveOnTopLeft);

			// If the offset is in a node after any visualized node go to the end of the list.
			if (i >= list->rangeEnd() )
				return list->moveCursor( Visualization::Item::MoveOnBottomRight);

			if ( setOffsetInItem(offset, list->itemAt<Visualization::Item>(i - list->rangeBegin())) )
				return true;
		}
		else
			offset -= components[i].size();
	}

	if (offset == 0) // This most likely means that the components are empty
		return list->moveCursor( Visualization::Item::MoveOnCenter);
	else // This means we've reached the end
		return list->moveCursor( Visualization::Item::MoveOnBottomRight);
}

int StringOffsetProvider::listItemOffset(Visualization::VList* list,
		const QString& prefix, const QString& separator, const QString& postfix, Qt::Key key)
{
	QStringList components = StringOffsetProvider::components(list->node());

	int result = prefix.size();
	if (list->scene()->mainCursor() && list->scene()->mainCursor()->owner() == list)
	{
		int index = list->correspondingSceneCursor<Visualization::LayoutCursor>()->index();

		if (index < 0) result = 0;
		else
		{
			// Adjust for the range but only if the cursor is inside the list
			index += list->rangeBegin();

			if (index > components.size())
			{
				result += postfix.size();
				--index;
			}

			for (int i = 0; i<index; ++i)
			{
				if (i>0) result += separator.size();

				result += components[i].size();
			}
		}
	}
	else
	{
		int focusedNodeIndex = list->focusedNodeIndex();
		Q_ASSERT(focusedNodeIndex >= 0);

		for (int i = 0; i<focusedNodeIndex; ++i)
			result += components[i].size() + separator.size();

		result += itemOffset(list->itemAt<Visualization::Item>(list->focusedItemIndex()),
				components[focusedNodeIndex].length(), key);
	}

	return result;
}

StringOffsetProvider* StringOffsetProvider::defaultProvider(Visualization::Item* item)
{
	if ( GridBasedOffsetProvider::hasGridConstructorfor (item)) return new GridBasedOffsetProvider{item};
	if ( auto tr = DCast<Visualization::TextRenderer>(item)) return new TextRendererStringOffsetProvider{tr};

	for (auto f : allowGridBasedProviderFunctions_)
		if (f(item))
		{
			if ( DCast<Visualization::LayoutProvider<>>(item) )
				return new GridBasedOffsetProvider{item};

			if (auto declarativeItem = DCast<Visualization::DeclarativeItemBase>(item))
			{
				if (dynamic_cast<Visualization::GridLayoutFormElement*>(declarativeItem->currentForm()))
					return new GridBasedOffsetProvider{item};
			}

			break;
		}

	return nullptr;
}

}
