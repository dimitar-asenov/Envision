/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * StringOffsetProvider.cpp
 *
 *  Created on: Feb 14, 2012
 *      Author: Dimitar Asenov
 */

#include "string_offset_providers/StringOffsetProvider.h"
#include "string_components/StringComponents.h"

#include "VisualizationBase/headers/cursor/LayoutCursor.h"
#include "VisualizationBase/headers/cursor/Cursor.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/items/Item.h"
#include "ModelBase/headers/adapter/AdapterManager.h"
#include "Core/headers/global.h"

namespace OOInteraction {

StringOffsetProvider::~StringOffsetProvider()
{
}

QStringList StringOffsetProvider::components(Model::Node* node)
{
	if (!node) return QStringList();

	QStringList result;
	StringComponents* sc = Model::AdapterManager::adapt<StringComponents>(node);
	if (sc)
	{
		result = sc->components();
		SAFE_DELETE(sc);
	}

	return result;
}

QString StringOffsetProvider::stringFromComponenets(Model::Node* node)
{
	auto c = components(node);
	return c.isEmpty() ? QString() : c.join("");
}

QString StringOffsetProvider::stringFromComponenets(Visualization::Item* item)
{
	if (!item) return QString();
	return stringFromComponenets(item->node());
}

QString StringOffsetProvider::stringFromStringOffsetProvider(Visualization::Item* item)
{
	if (!item) return QString();

	QString result;
	StringOffsetProvider* sp = Model::AdapterManager::adapt<StringOffsetProvider>(item);
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
	auto child = Model::AdapterManager::adapt<StringOffsetProvider>(item);
	if (child)
	{
		if (offset > 0 && child->isIndivisible()) child->setOffset(child->string().length());
		else child->setOffset(offset);
		SAFE_DELETE(child);
		return true;
	}
	return false;
}

int StringOffsetProvider::itemOffset(Visualization::Item* item, int stringComponentLenght)
{
	StringOffsetProvider* child = Model::AdapterManager::adapt<StringOffsetProvider>(item);
	int offset = 0;
	if (child)
	{
		offset = child->offset();
		if (offset > 0 && child->isIndivisible()) offset = stringComponentLenght;
		SAFE_DELETE(child);
	}
	return offset;
}

bool StringOffsetProvider::setOffsetInListItem(int& offset, Visualization::VList* list,
		const QString& prefix, const QString& separator, const QString& /*postfix*/)
{
	QStringList components = StringOffsetProvider::components(list->node());

	offset -= prefix.size();
	for (int i = 0; i<list->length(); ++i)
	{
		if (i>0) offset -= separator.size();

		if (offset <= components[i].size())
		{
			if ( setOffsetInItem(offset, list->at<Visualization::Item>(i)) )
				return true;
		}
		else
			offset -= components[i].size();
	}

	return false;
}

int StringOffsetProvider::listItemOffset(Visualization::VList* list,
		const QString& prefix, const QString& separator, const QString& /*postfix*/)
{
	QStringList components = StringOffsetProvider::components(list->node());

	int result = prefix.size();
	if (list->scene()->mainCursor() && list->scene()->mainCursor()->owner() == list->layout())
	{
		int index = list->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		for (int i = 0; i<index; ++i)
		{
			if (i>0) result += separator.size();
			result += components[i].size();
		}
	}
	else
	{
		int focused = list->focusedElementIndex();
		Q_ASSERT(focused >= 0);

		for(int i = 0; i<focused; ++i)
			result += components[i].size() + separator.size();

		result += itemOffset(list->at<Visualization::Item>(focused), components[focused].length());
	}

	return result;
}

} /* namespace OOInteraction */
