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
 * NewArrayStringOffsetProvider.cpp
 *
 *  Created on: Mar 14, 2012
 *      Author: Dimitar Asenov
 */

#include "string_offset_providers/NewArrayStringOffsetProvider.h"
#include "OOVisualization/src/expressions/VNewExpression.h"

namespace OOInteraction {

NewArrayStringOffsetProvider::NewArrayStringOffsetProvider(OOVisualization::VNewExpression* vis)
	: StringOffsetProvider(vis), vis_(vis)
{
}

int NewArrayStringOffsetProvider::offset()
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;

	QStringList components = this->components();

	int result = 0;

	int focused = vis_->layout()->focusedElementIndex();
	int componentIndex = 0;
	if (focused == 0) componentIndex = 4;
	else if (focused == 1) componentIndex = 0;
	else componentIndex = 2;
	result += itemOffset(vis_->layout()->at<Visualization::Item>(focused), components[componentIndex].length());

	if (focused == 0) for (int i = 0; i< 4; ++i ) result += components[i].size();
	else if (focused == 2) for (int i = 0; i< 2; ++i ) result += components[i].size();

	return result;
}

void NewArrayStringOffsetProvider::setOffset(int offset)
{
	QStringList components = this->components();

	int childIndex = 1; // Corresponds to the second child - 'new' symbol
	if (offset > components[0].size())
	{
		offset -= components[0].size() + components[1].size(); // components[1] for the '_' between new and the type name
		childIndex = 2;
		if (offset > components[2].size())
		{
			// components[3] for the '[' between new and the type name
			offset -= components[2].size() + components[3].size();
			childIndex = 0;
			if (offset > components[4].size())
			{
				// This happened when we just finished typing
				childIndex = 2;
				offset = components[2].size();
			}
		}
	}

	setOffsetInItem(offset, vis_->layout()->at<Visualization::Item>(childIndex));
}

} /* namespace OOInteraction */
