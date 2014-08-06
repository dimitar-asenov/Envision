/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "Commit.h"

namespace FilePersistence {

CommitFile::CommitFile()
{
	relativePath_ = QString();
	size_ = 0;
	content_ = nullptr;
}

CommitFile::CommitFile(QString relativePath, qint64 size, const char* content)
{
	relativePath_ = relativePath;
	size_ = size;
	content_ = content;
}

CommitFile::~CommitFile()
{
	delete[] content_;
}

Commit::Commit() {}

Commit::~Commit()
{
	for (auto file : files_.values())
		delete file;
}

void Commit::setMetaData(CommitMetaData data)
{
	information_ = data;
}

CommitMetaData Commit::metaData() const
{
	return information_;
}

void Commit::addFile(QString relativePath, qint64 size, const char* content)
{
	files_.insert(relativePath, new CommitFile(relativePath, size, content));
}

bool Commit::getFileContent(QString fileName, const char*& content, int& contentSize) const
{
	QHash<QString, CommitFile*>::const_iterator iter = files_.find(fileName);
	if (iter != files_.constEnd())
	{
		contentSize = iter.value()->size_;
		content = iter.value()->content_;

		return true;
	}
	else
		return false;
}

} /* namespace FilePersistence */
