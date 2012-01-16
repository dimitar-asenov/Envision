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
 * NodeIdMap.cpp
 *
 *  Created on: Jan 16, 2012
 *      Author: Dimitar Asenov
 */

#include "NodeIdMap.h"
#include "FilePersistenceException.h"

namespace FilePersistence {

NodeIdMap::NodeIdMap()
	: nextId_(0)
{
}

NodeIdMap::NodeIdType NodeIdMap::getId(const Model::Node* node)
{
	QMap<const Model::Node*, NodeIdType>::const_iterator iter = map.find(node);
	if ( iter != map.end() ) return *iter;

	int id = nextId_++;
	map.insert(node, id);
	return id;
}

void NodeIdMap::setId(const Model::Node* node, NodeIdType id)
{
	QMap<const Model::Node*, NodeIdType>::const_iterator iter = map.find(node);
	if (iter != map.end())
	{
		if (*iter == id) return;
		else throw new FilePersistenceException("Setting an inconsistent node id in NodeIdMap");
	}

	map.insert(node, id);
}

} /* namespace FilePersistence */
