/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "ExportMapContainer.h"

#include "ModelBase/src/nodes/Node.h"

namespace Export {

int ExportMapContainer::getLatestDirRevision(Model::Node* node) const
{
	Q_ASSERT(node);
	auto search = dirMap_.constFind(node);
	if (search != dirMap_.constEnd())
		return search.value().lastKey();
	return -1;
}

std::shared_ptr<SourceDir> ExportMapContainer::getLatestDir(Model::Node* node) const
{
	Q_ASSERT(node);
	auto search = dirMap_.constFind(node);
	if (search != dirMap_.constEnd())
		return search.value().last();
	return nullptr;
}

void ExportMapContainer::insertDir(Model::Node* node, std::shared_ptr<SourceDir> dir)
{
	Q_ASSERT(node);
	dirMap_[node].insert(node->revision(), dir);
}

void ExportMapContainer::clearAllDirs(Model::Node* node)
{
	Q_ASSERT(node);
	auto search = dirMap_.find(node);
	if (search != dirMap_.end())
		search.value().clear();
}

int ExportMapContainer::getLatestTextToNodeRevision(Model::Node* node) const
{
	Q_ASSERT(node);
	auto search = textToNodeMap_.constFind(node);
	if (search != textToNodeMap_.constEnd())
		return search.value().lastKey();
	return -1;
}

std::shared_ptr<TextToNodeMap> ExportMapContainer::getLatestTextToNode(Model::Node* node) const
{
	Q_ASSERT(node);
	auto search = textToNodeMap_.constFind(node);
	if (search != textToNodeMap_.constEnd())
		return search.value().last();
	return nullptr;
}

void ExportMapContainer::insertTextToNode(Model::Node* node, std::shared_ptr<TextToNodeMap> map)
{
	Q_ASSERT(node);
	textToNodeMap_[node].insert(node->revision(), map);
}

void ExportMapContainer::clearAllTextToNode(Model::Node* node)
{
	Q_ASSERT(node);
	auto search = textToNodeMap_.find(node);
	if (search != textToNodeMap_.end())
		search.value().clear();
}

} /* namespace Export */
