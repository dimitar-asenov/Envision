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
 * CallStringProvider.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: Dimitar Asenov
 */

#include "string_providers/CallStringProvider.h"
#include "string_components/StringComponents.h"

#include "OOVisualization/headers/expressions/VMethodCallExpression.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/cursor/LayoutCursor.h"
#include "VisualizationBase/headers/cursor/Cursor.h"
#include "ModelBase/headers/adapter/AdapterManager.h"

namespace OOInteraction {

CallStringProvider::CallStringProvider(OOVisualization::VMethodCallExpression* vis)
	: SequentialVisualizationStringProvider(vis), vis_(vis)
{
}

QStringList CallStringProvider::components()
{
	QStringList components = detailedComponents();

	int start = components.indexOf("(");
	int end = components.lastIndexOf(")");
	for(int i = start; i<end; ++i)
	{
		components[start] += components[start+1];
		components.removeAt(start+1);
	}

	return components;
}

QStringList CallStringProvider::detailedComponents()
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

int CallStringProvider::offset()
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;

	if (!vis_->arguments()->itemOrChildHasFocus())
		return SequentialVisualizationStringProvider::offset();

	QStringList components = detailedComponents();
	int result = 0;

	int index = 0;
	while(index == 0 || components[index-1] != "(")
		result += components[index++].size();


	if (vis_->scene()->mainCursor() && vis_->scene()->mainCursor()->owner() == vis_->arguments()->layout())
	{
		int argIndex = vis_->arguments()->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		for (int i = 0; i<argIndex; ++i)
		{
			if (i>0) result += components[index++].size(); // Add previous comma
			result += components[index++].size();
		}

	}
	else
	{
		int focused = vis_->arguments()->focusedElementIndex();
		Q_ASSERT(focused >= 0);

		for(int i = 0; i<focused; ++i)
		{
			result += components[index].size() + components[index+1].size();
			index += 2;
		}

		StringProvider* child =
				Model::AdapterManager::adapt<StringProvider>(vis_->arguments()->at<Visualization::Item>(focused));
		if (child)
		{
			int childOffset = child->offset();
			if (childOffset > 0 && child->isIndivisible()) childOffset = components[focused].length();
			result += childOffset;
			SAFE_DELETE(child);
		}
	}

	return result;
}

QString CallStringProvider::string()
{
	if (!vis_) return QString();
	return components().join("");
}

void CallStringProvider::setOffset(int offset)
{
	if (offset == 0)
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(0,0));
		return;
	}

	QStringList components = detailedComponents();

	if (offset == components.join("").size())
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(vis_->xEnd(),0));
		return;
	}

	int index = 0;
	int argsStart = 0;
	while(index == 0 || components[index-1] != "(")
		argsStart += components[index++].size();

	if (offset < argsStart)
	{
		SequentialVisualizationStringProvider::setOffset(offset);
		return;
	}
	else offset -= argsStart;

	for (int i = 0; i<vis_->arguments()->length(); ++i)
	{
		if (offset <= components[index].size())
		{
			StringProvider* child =
					Model::AdapterManager::adapt<StringProvider>(vis_->arguments()->at<Visualization::Item>(i));
			if (child)
			{
				if (offset > 0 && child->isIndivisible()) child->setOffset(child->string().length());
				else child->setOffset(offset);
				SAFE_DELETE(child);
				return;
			}
		}
		else
		{
			offset -= components[index++].size();
			offset -= components[index++].size(); // This is the ',' after the argument
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
