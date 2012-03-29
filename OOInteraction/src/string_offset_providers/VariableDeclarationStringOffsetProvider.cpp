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
 * VariableDeclarationStringOffsetProvider.cpp
 *
 *  Created on: Mar 19, 2012
 *      Author: Dimitar Asenov
 */

#include "string_offset_providers/VariableDeclarationStringOffsetProvider.h"

#include "VisualizationBase/src/cursor/LayoutCursor.h"

namespace OOInteraction {

VariableDeclarationStringOffsetProvider::VariableDeclarationStringOffsetProvider
	(Visualization::LayoutProvider<Visualization::SequentialLayout>* vis)
	: StringOffsetProvider(vis), vis_(vis)
{
}

QStringList VariableDeclarationStringOffsetProvider::components()
{
	QStringList components = StringOffsetProvider::components();

	if (components.size() != vis_->layout()->length())
	{
		for (int i = components.size() - 1; i>=0; --i)
			if (components[i].isNull())
				components.removeAt(i);
	}
	if (components.size() != vis_->layout()->length())
		components.removeAll(QString(""));

	// Remove the space character between the type and the name of the variable
	if (components.size() != vis_->layout()->length())
		components.removeAll(QString(" "));

	return components;
}

int VariableDeclarationStringOffsetProvider::offset()
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;

	QStringList components = this->components();
	Q_ASSERT(components.size() == vis_->layout()->length());

	int result = 0;

	if (vis_->scene()->mainCursor() && vis_->scene()->mainCursor()->owner() == vis_->layout())
	{
		int index = vis_->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		for (int i = 0; i<index; ++i)
			result += components[i].size();

		if (index > 1) result += 1; // This is for the ' ' character between the type and name.
	}
	else
	{
		int focused = vis_->layout()->focusedElementIndex();
		for(int i = 0; i<focused; ++i)
			result += components[i].size();

		if (focused > 0) result += 1; // This is for the ' ' character between the type and name.

		result += itemOffset(vis_->layout()->at<Visualization::Item>(focused), components[focused].length());
	}

	return result;
}

void VariableDeclarationStringOffsetProvider::setOffset(int offset)
{
	if (offset == 0)
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(0,0));
		return;
	}

	QStringList components = this->components();
	Q_ASSERT(components.size() == vis_->layout()->length());
	for (int i = 0; i<vis_->layout()->length(); ++i)
	{
		if (i == 1) offset -= 1; // Remove the offset for the space between the type and the name.

		if (offset < components[i].size())
		{
			if ( setOffsetInItem(offset, vis_->layout()->at<Visualization::Item>(i)) )
				return;
		}
		else
		{
			offset -= components[i].size();
			if (offset == 0)
			{
				QPoint reference(vis_->layout()->at<Visualization::Item>(i)->xEnd()+1, 0);
				if (reference.x() > vis_->layout()->width()-1 ) reference.setX(vis_->layout()->width() - 1);
				reference += vis_->layout()->pos().toPoint();
				vis_->moveCursor( Visualization::Item::MoveOnPosition, reference);
				return;
			}
		}
	}

	Q_ASSERT(false);
}

} /* namespace OOInteraction */
