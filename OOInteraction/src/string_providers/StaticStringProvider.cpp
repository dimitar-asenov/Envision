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
 * StaticStringProvider.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: Dimitar Asenov
 */

#include "string_providers/StaticStringProvider.h"

#include "VisualizationBase/headers/items/Static.h"
#include "ModelBase/headers/adapter/AdapterManager.h"

namespace OOInteraction {

StaticStringProvider::StaticStringProvider(Visualization::Static* v)
: vis_(v)
{
}

int StaticStringProvider::offset()
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;


	int result = 0;
	StringProvider* child =
			Model::AdapterManager::adapt<StringProvider>(vis_->item());
	if (child)
	{
		result = child->offset();
		SAFE_DELETE(child);
	}

	return result;
}

QString StaticStringProvider::string()
{
	if (!vis_) return QString();

	QString result = 0;
	StringProvider* child =
			Model::AdapterManager::adapt<StringProvider>(vis_->item());
	if (child)
	{
		result = child->string();
		SAFE_DELETE(child);
	}

	return result;
}

void StaticStringProvider::setOffset(int offset)
{
	if (!vis_) return;

	StringProvider* child =
			Model::AdapterManager::adapt<StringProvider>(vis_->item());
	if (child)
	{
		child->setOffset(offset);
		SAFE_DELETE(child);
	}
}

} /* namespace OOInteraction */
