/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "CommitGraph.h"

namespace FilePersistence {

CommitGraphItem::CommitGraphItem(QString sha1)
	: commitSHA1_{sha1}
{}

CommitGraph::CommitGraph(QString start, QString end)
{
	auto iterator = items_.insert(start, CommitGraphItem{start});
	start_ = &iterator.value();

	iterator = items_.insert(end, CommitGraphItem{end});
	end_ = &iterator.value();
}

void CommitGraph::add(QString fromCommitSHA1, QString toCommitSHA1)
{
	CommitGraphItem* fromItem = nullptr;
	CommitGraphItem* toItem = nullptr;

	auto iterator = items_.find(fromCommitSHA1);
	if (iterator == items_.end())
		iterator = items_.insert(fromCommitSHA1, CommitGraphItem{fromCommitSHA1});
	fromItem = &iterator.value();


	iterator = items_.find(toCommitSHA1);
	if (iterator == items_.end())
		iterator = items_.insert(toCommitSHA1, CommitGraphItem{toCommitSHA1});
	toItem = &iterator.value();

	fromItem->children_.append(toItem);
	toItem->parents_.append(fromItem);
}

const CommitGraphItem* CommitGraph::find(QString commit) const
{
	QHash<QString, CommitGraphItem>::const_iterator iter = items_.find(commit);
	if (iter == items_.constEnd())
		return nullptr;
	else
		return &iter.value();
}

}
