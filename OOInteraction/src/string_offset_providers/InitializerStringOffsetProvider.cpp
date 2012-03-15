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
 * InitializerStringOffsetProvider.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: Dimitar Asenov
 */

#include "string_offset_providers/InitializerStringOffsetProvider.h"
#include "string_components/StringComponents.h"

#include "OOVisualization/headers/expressions/VArrayInitializer.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/layouts/GridLayout.h"
#include "VisualizationBase/headers/cursor/LayoutCursor.h"
#include "VisualizationBase/headers/cursor/Cursor.h"
#include "ModelBase/headers/adapter/AdapterManager.h"

namespace OOInteraction {

InitializerStringOffsetProvider::InitializerStringOffsetProvider(OOVisualization::VArrayInitializer* vis)
	: vis_(vis)
{
}

QStringList InitializerStringOffsetProvider::components()
{
	QStringList components;
	StringComponents* node = Model::AdapterManager::adapt<StringComponents>(vis_->node());
	if (node)
	{
		components = node->components();
		SAFE_DELETE(node);
	}

	return components;
}

int InitializerStringOffsetProvider::offset()
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;

	QStringList components = this->components();
	int result = 0;

	// Always include the leading '{' in the returned offset
	result += components[0].size();

	if (vis_->isShownInMatrixForm())
	{
		auto focusedIndex = vis_->layout()->focusedElementIndex();

		int index = 1;
		for(int y = 0; y < focusedIndex.y(); ++y)
		{
			result += components[index++].size();
			result += components[index++].size(); // This is for the comma after the element
		}

		QStringList subComponents = StringOffsetProvider::components(vis_->node()->values()->at(focusedIndex.y()));
		result += subComponents[0].size();

		int subIndex = 1;
		for(int x = 0; x<focusedIndex.x(); ++x)
		{
			result += subComponents[subIndex++].size();
			result += subComponents[subIndex++].size();
		}

		result += itemOffset(vis_->layout()->focusedChild(), subComponents[subIndex].size());
	}
	else
	{
		Q_ASSERT( (components.size() - 1) / 2 == vis_->values()->length() );

		if (vis_->scene()->mainCursor() && vis_->scene()->mainCursor()->owner() == vis_->values()->layout())
		{
			int index = vis_->values()->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
			for (int i = 0; i<index; ++i)
			{
				if (i>0) result += components[i*2].size(); //include previous comma
				result += components[i*2 + 1].size();
			}
		}
		else
		{
			int focused = vis_->values()->focusedElementIndex();
			Q_ASSERT(focused >= 0);

			for(int i = 0; i<focused; ++i)
				result += components[(i+1)*2].size() + components[i*2 + 1].size();

			result += itemOffset(vis_->values()->at<Visualization::Item>(focused), components[focused].length());
		}

	}

	return result;
}

QString InitializerStringOffsetProvider::string()
{
	if (!vis_) return QString();
	return components().join("");
}

void InitializerStringOffsetProvider::setOffset(int offset)
{
	if (offset == 0)
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(0,0));
		return;
	}

	QStringList components = this->components();

	if (vis_->isShownInMatrixForm())
	{
		// TODO: implement matrix form
		int index = 0;
		offset -= components[index++].size();

		while (index < components.size() && offset > components[index].size())
		{
			offset -= components[index++].size();
			offset -= components[index++].size();
		}

		if (offset < 0) offset = 0;
		if (index == components.size()) index -= 2;

		QStringList subComponents = StringOffsetProvider::components(vis_->node()->values()->at((index-1)/2));

		int subIndex = 0;
		offset -= subComponents[subIndex++].size();
		if (offset < 0) offset = 0;
		while(subIndex < subComponents.size() && offset > subComponents[subIndex].size())
		{
			offset -= subComponents[subIndex++].size();
			offset -= subComponents[subIndex++].size();
		}

		if (offset < 0) offset = 0;
		if (subIndex == subComponents.size()) subIndex -= 2;

		auto childItem = vis_->layout()->at<Visualization::Item>((subIndex-1)/2,(index-1)/2);
		if ( setOffsetInItem(offset, childItem) )
			return;
	}
	else
	{
		Q_ASSERT( (components.size() - 1) / 2 == vis_->values()->length() );
		for (int i = 0; i<vis_->values()->length(); ++i)
		{
			offset -= components[i*2].size();  // This is to cover for the inital '{' and subsequent ','

			if (offset <= components[i*2 + 1].size()) // We only care about the components which represent children
			{
				if ( setOffsetInItem(offset, vis_->values()->at<Visualization::Item>(i)) )
					return;
			}
			else
			{
				offset -= components[i*2 + 1].size();
			}
		}
	}

	if (offset == components.last().size())
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(vis_->xEnd(),0));
	}
	else
		Q_ASSERT(false);
}

} /* namespace OOInteraction */
