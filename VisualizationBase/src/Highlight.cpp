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
#include "Highlight.h"
#include "items/SelectedItem.h"

namespace Visualization {

Highlight::Highlight(Scene* scene, const QString& name, const QString& styleName)
: scene_{scene}, name_{name}, styleName_{styleName}
{
	Q_ASSERT(scene_);
	Q_ASSERT(!name.isEmpty());
}

Highlight::~Highlight()
{
	for (auto sel : highlightItems_) SAFE_DELETE_ITEM(sel);
}

void Highlight::addHighlightedItem(Item* item)
{
	auto highlight = new SelectedItem(item, SelectedItem::itemStyles().get(styleName_));
	highlightItems_.insert(item, highlight );
	scene_->addItem(highlight);
	scene_->scheduleUpdate();
}

void Highlight::updateAllHighlights()
{
	for (auto it = highlightItems_.begin(); it != highlightItems_.end(); ++it)
		it.value()->updateSubtree();
}

void Highlight::removeHighlightedItem(Item* item)
{
	highlightItems_.remove(item);
}

} /* namespace Visualization */
