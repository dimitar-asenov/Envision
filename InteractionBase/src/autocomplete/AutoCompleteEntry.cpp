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
 * AutoCompleteEntry.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: Dimitar Asenov
 */

#include "AutoCompleteEntry.h"

#include "VisualizationBase/src/items/Item.h"

namespace Interaction {

AutoCompleteEntry::AutoCompleteEntry(const QString& text, const QString& description,
		Visualization::Item* visualization, ExecuteFunction execFunction) :
	text_(text), description_(description), vis_(visualization), execFunction_(execFunction)
{
}

AutoCompleteEntry::~AutoCompleteEntry()
{
	SAFE_DELETE_ITEM(vis_);
}


void AutoCompleteEntry::setVisualization(Visualization::Item* item)
{
	SAFE_DELETE_ITEM(vis_);
	vis_ = item;
}

void AutoCompleteEntry::setExecutionFunction(ExecuteFunction execFunction)
{
	execFunction_ = execFunction;
}

void AutoCompleteEntry::execute()
{
	if (execFunction_) execFunction_(this);
}

} /* namespace Interaction */
