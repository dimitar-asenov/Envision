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

#include "StaticStringOffsetProvider.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/TextRenderer.h"

namespace OOInteraction {

StaticStringOffsetProvider::StaticStringOffsetProvider(Visualization::Static* v)
: StringOffsetProvider(v), vis_(v)
{
}

int StaticStringOffsetProvider::offset(Qt::Key key)
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;
	return itemOffset(vis_->item(), string().size(), key);
}

QString StaticStringOffsetProvider::string()
{
	return stringFromStringOffsetProvider(vis_? vis_->item() : nullptr);
}

void StaticStringOffsetProvider::setOffset(int offset)
{
	setOffsetInItem(offset, vis_->item());
}

bool StaticStringOffsetProvider::isIndivisible()
{
	Visualization::TextRenderer* tr = nullptr;
	if (vis_)
	{
		auto ci = vis_->childItems();
		while (ci.length() == 1)
		{
			tr =  DCast<Visualization::TextRenderer> (ci.first());
			if (tr) break;

			ci = ci.first()->childItems();
		}
	}

	return !tr || tr->text().length() != string().length();
}

} /* namespace OOInteraction */
