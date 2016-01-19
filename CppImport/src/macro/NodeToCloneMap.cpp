/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "NodeToCloneMap.h"

namespace CppImport {

void NodeToCloneMap::add(Model::Node* original, Model::Node* clone)
{
	Q_ASSERT(original);
	Q_ASSERT(clone);
	clones_[clone] = original;
	originals_[original] = clone;
}

Model::Node*NodeToCloneMap::original(Model::Node* clone)
{
	auto it = clones_.find(clone);

	return it != clones_.end() ? *it : nullptr;
}

QList<Model::Node*> NodeToCloneMap::original(QList<Model::Node*> clones)
{
	QList<Model::Node*> result;
	for (auto clone : clones)
		result.append(original(clone));
	return result;
}

Model::Node*NodeToCloneMap::clone(Model::Node* original)
{
	auto it = originals_.find(original);

	return it != originals_.end() ? *it : nullptr;
}

QList<Model::Node*> NodeToCloneMap::clone(QList<Model::Node*> originals)
{
	QList<Model::Node*> result;
	for (auto original : originals)
		result.append(clone(original));
	return result;
}

void NodeToCloneMap::replaceClone(Model::Node* old, Model::Node* replacement)
{
	if (auto original = clone(old))
	{
		clones_.remove(old);
		add(original, replacement);
	}
}

}
