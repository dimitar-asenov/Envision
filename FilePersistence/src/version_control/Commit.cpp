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

#include "Commit.h"

namespace FilePersistence {

Signature::Signature()
{
	dateTime_ = QDateTime::currentDateTimeUtc();
	timeZone_ = QTimeZone{QTimeZone::systemTimeZoneId()};
}

CommitFile::CommitFile(){}

CommitFile::CommitFile(QString relativePath, qint64 size, std::unique_ptr<char[]> content)
	: relativePath_{relativePath}, size_{size}, content_{std::move(content)}
{}

CommitFile::CommitFile(QString relativePath, qint64 size, std::unique_ptr<char[], CommitFileContentDeleter> content)
	: relativePath_{relativePath}, size_{size}, contentWithDeleter_{std::move(content)}
{}

const char* CommitFile::content() const
{
	Q_ASSERT((content_ && !contentWithDeleter_) || (!content_ && contentWithDeleter_));
	if (content_)
		return content_.get();
	else
		return contentWithDeleter_.get();
}

Commit::Commit() {}

Commit::~Commit()
{
	for (auto file : files_.values())
		SAFE_DELETE(file);
}

void Commit::addFile(QString relativePath, qint64 size, std::unique_ptr<char[]> content)
{
	files_.insert(relativePath, new CommitFile{relativePath, size, std::move(content)});
}

void Commit::addFile(QString relativePath, qint64 size, std::unique_ptr<char[], CommitFileContentDeleter> content)
{
	files_.insert(relativePath, new CommitFile{relativePath, size, std::move(content)});
}

bool Commit::getFileContent(QString fileName, const char*& content, int& contentSize, bool exactFileNameMatching) const
{
	QHash<QString, CommitFile*>::const_iterator iter;

	// name of file must match fileName exactly
	if (exactFileNameMatching)
		iter = files_.find(fileName);
	// name of file must contain fileName
	else
	{
		iter = files_.constBegin();
		while (iter != files_.constEnd())
		{
			QFileInfo fileInfo{iter.key()};
			if (!fileInfo.isDir() &&
				 ((fileName.startsWith("{") && fileInfo.fileName().contains(fileName))
				 || fileInfo.fileName() == fileName))
				break;
			iter++;
		}
	}

	if (iter != files_.constEnd())
	{
		contentSize = iter.value()->size_;
		content = iter.value()->content();
		return true;
	}
	else
		return false;
}

QStringList Commit::nodeLinesFromId(Model::NodeIdType id, bool findChildrenByParentId) const
{
	auto idText =  id.toString();
	QStringList matches;
	for (auto file : files())
	{
		int indexOfId = 0;
		auto content = QString::fromUtf8(file->content());

		indexOfId = content.indexOf(idText, indexOfId);
		if (indexOfId != -1)
		{
			bool invalid = false;
			auto start = indexOfId;
			auto end = indexOfId;

			// start is the first character of the line containing id
			while (start >= 0 && content[start] != '\n')
			{
				// String is of the form {.*} {id}
				if (!findChildrenByParentId)
					if (content[start] == '}')
					{
						invalid = true;
						break;
					}
				start--;
			}
			if (invalid)	continue;
			start++;

			// end is the character after the line containing id
			while (end <= content.size() && content[end] != '\n')
			{
				// String is of the form {id} {*.}
				if (findChildrenByParentId)
					if (content[end] == '{')
					{
						invalid = true;
						break;
					}
				end++;
			}
			if (invalid)	continue;

			QString match = file->relativePath_ + ":" + content.mid(start, end-start);
			matches << match;

			// Find the next match
			indexOfId = indexOfId + 1;
		}
	}

	return matches;
}

}
