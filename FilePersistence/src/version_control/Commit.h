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

#pragma once

#include "../filepersistence_api.h"
#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

struct FILEPERSISTENCE_API Signature {
		QString name_;
		QString eMail_;
		QDateTime dateTime_;
		QTimeZone timeZone_;

		Signature();
};

struct FILEPERSISTENCE_API CommitMetaData {
		QString sha1_;
		QString message_;
		QDateTime dateTime_;
		Signature committer_;
		Signature author_;
};

using CommitFileContentDeleter = std::function<void (char*)>;

struct FILEPERSISTENCE_API CommitFile {
		QString relativePath_;
		qint64 size_{};

		CommitFile();
		CommitFile(QString relativePath, qint64 size, std::unique_ptr<char[]> content);
		CommitFile(QString relativePath, qint64 size, std::unique_ptr<char[], CommitFileContentDeleter> content);

		const char* content() const;

	private:
		std::unique_ptr<char[]> content_;
		std::unique_ptr<char[], CommitFileContentDeleter> contentWithDeleter_;
};

class FILEPERSISTENCE_API Commit
{
	public:
		Commit();
		~Commit();

		void setMetaData(CommitMetaData data);
		CommitMetaData metaData() const;

		QList<CommitFile*> files() const;

		void addFile(QString relativePath, qint64 size, std::unique_ptr<char[]> content);
		void addFile(QString relativePath, qint64 size, std::unique_ptr<char[], CommitFileContentDeleter> content);

		bool getFileContent(QString fileName, const char*& content, int& contentSize, bool exactFileNameMatching) const;
		bool isValidMatch(QString content, int indexOfId, int& start, int& end, bool findChildrenByParentId) const;
		QStringList nodeLinesFromId(Model::NodeIdType id, bool findChildrenByParentId = false) const;


	private:
		CommitMetaData information_;

		QHash<QString, CommitFile*> files_;

};

inline QList<CommitFile*> Commit::files() const { return files_.values(); }

inline void Commit::setMetaData(CommitMetaData data) { information_ = data; }
inline CommitMetaData Commit::metaData() const { return information_; }

}
