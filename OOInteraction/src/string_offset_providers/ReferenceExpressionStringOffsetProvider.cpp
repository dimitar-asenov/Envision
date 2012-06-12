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
 * ReferenceExpressionStringOffsetProvider.cpp
 *
 *  Created on: May 30, 2012
 *      Author: Dimitar Asenov
 */

#include "ReferenceExpressionStringOffsetProvider.h"

#include "OOVisualization/src/expressions/VReferenceExpression.h"
#include "VisualizationBase/src/items/VList.h"

namespace OOInteraction {

ReferenceExpressionStringOffsetProvider
	::ReferenceExpressionStringOffsetProvider(OOVisualization::VReferenceExpression* vis)
	: SequentialVisualizationStringOffsetProvider(vis), vis_(vis)
{
}

int ReferenceExpressionStringOffsetProvider::offset(Qt::Key key)
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;

	if (vis_->typeArguments() == nullptr || !vis_->typeArguments()->itemOrChildHasFocus())
		return SequentialVisualizationStringOffsetProvider::offset(key);

	QStringList components = this->components();
	int result = 0;

	int listIndex = (components.size() > 1 && components[1] == ".") ? 3 : 1;
	for(int i = 0; i< listIndex; ++i)
		result += components[i].size();

	result += listItemOffset(vis_->typeArguments(),"<", ",", ">", key);
	return result;
}

void ReferenceExpressionStringOffsetProvider::setOffset(int offset)
{
	if (offset == 0)
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(0,0));
		return;
	}

	QStringList components = this->components();

	if (offset == components.join("").size())
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(vis_->width()-1,0));
		return;
	}

	int listOffest = 0;
	int listIndex = (components.size() > 1 && components[1] == ".") ? 3 : 1;
	for(int i = 0; i< listIndex; ++i)
		listOffest += components[i].size();

	if (components.size() == listIndex || offset <= listOffest)
	{
		SequentialVisualizationStringOffsetProvider::setOffset(offset);
		return;
	}
	else offset -= listOffest;

	if ( setOffsetInListItem(offset, vis_->typeArguments(), "<", ",", ">"))
		return;

	if (offset == components.last().size())
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(vis_->width()-1,0));
	else
		Q_ASSERT(false);
}

} /* namespace OOInteraction */
