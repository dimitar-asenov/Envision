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
 * ContractCallOffsetProvider.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: Dimitar Asenov
 */

#include "ContractCallOffsetProvider.h"
#include "../items/VContractCall.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VList.h"

namespace ContractsLibrary {

ContractCallOffsetProvider::ContractCallOffsetProvider(VContractCall* vis)
	: StringOffsetProvider(vis), vis_(vis)
{
}

int ContractCallOffsetProvider::offset()
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;

	QStringList components = this->components();
	int offset = itemOffset(vis_->keyword(), components[0].size());

	if (offset >=0)
	{
		if (offset < components[0].size())
		{
			int index = components[0].lastIndexOf('.');
			if (index >= 0) offset += index + 1;
		}
		return offset;
	}
	offset = components[0].size();

	offset += listItemOffset(vis_->arguments(),"(", ",", ")");

	return offset;
}

void ContractCallOffsetProvider::setOffset(int offset)
{
	if (offset == 0)
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(0,0));
		return;
	}

	QStringList components = this->components();

	if (offset == components.join("").size())
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(vis_->xEnd(),0));
		return;
	}

	int listOffest = components[0].size();

	if (offset <= listOffest)
	{
		int dot_index = components[0].lastIndexOf('.');
		int header_size = 0;
		if (dot_index >=0 ) header_size = dot_index + 1;

		offset -= header_size;
		if (offset <0) offset = 0;

		setOffsetInItem(offset, vis_->keyword());
		return;
	}
	else offset -= listOffest;

	if ( setOffsetInListItem(offset, vis_->arguments(), "(", ",", ")"))
		return;

	if (offset == components.last().size())
	{
		vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(vis_->xEnd(),0));
	}
	else
		Q_ASSERT(false);
}

} /* namespace ContractsLibrary */
