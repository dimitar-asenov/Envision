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

#pragma once

#include "../export_api.h"

namespace Model {
	class Node;
}

namespace Export {

struct EXPORT_API Span
{
	int startLine_{};
	int startColumn_{};
	int endLine_{};
	int endColumn_{};

	Span(int startLine, int startColumn, int endLine, int endColumn);
	bool includes(int line, int column) const;
};
inline Span::Span(int startLine, int startColumn, int endLine, int endColumn)
	: startLine_{startLine}, startColumn_{startColumn}, endLine_{endLine}, endColumn_{endColumn}{}
inline bool Span::includes(int line, int column) const
{
	bool startOk = startLine_ < line || (startLine_ == line && startColumn_ <= column);
	bool endOk = line < endLine_ || (line == endLine_ && column <= endColumn_);
	return startOk && endOk;
}

struct EXPORT_API SourceLocation
{
	QString filename_;
	Span span_;
};

class EXPORT_API TextToNodeMap {
	public:
		TextToNodeMap();

		Model::Node* node(const QString& fileName, int line, int column) const;
		QList<SourceLocation> locations(Model::Node* node) const;
		QStringList files() const;

		void add(Model::Node* node, SourceLocation location);

	private:
		QMultiHash<Model::Node*, SourceLocation> nodeToLocation_;
		QHash<QString, QList<QPair<Span, Model::Node*>>> filenameToSpans;
		QStringList mappedFiles_;
};

inline QStringList TextToNodeMap::files() const { return mappedFiles_; }

}
