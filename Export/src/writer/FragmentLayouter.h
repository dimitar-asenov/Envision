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
#include "TextToNodeMap.h"

namespace Model {
	class Node;
}

namespace Export {

class SourceFile;
class SourceFragment;

class EXPORT_API FragmentLayouter {
	public:
		FragmentLayouter(const QString& indentation);

		enum IndentationFlag {
			NoIndentation = 0x0,
			IndentPrePostFix = 0x1,
			IndentChildFragments = 0x2,
			SpaceAfterPrefix = 0x4,
			NewLineAfterPrefix = 0x8,
			SpaceBeforePostfix = 0x10,
			NewLineBeforePostfix = 0x20,
			NewLineAfterPostfix = 0x40,
			SpaceBeforeSeparator = 0x80,
			SpaceAfterSeparator = 0x100,
			EmptyLineAtEnd = 0x200
		};
		Q_DECLARE_FLAGS(IndentationFlags, IndentationFlag)

		void addRule(const QString& fragmentType, IndentationFlags parameters);

		QString render(SourceFile* file, TextToNodeMap* map);

	private:
		QString indentation_;
		QHash<QString, IndentationFlags> rules_;

		// These are only used while rendering a source file
		QString fileName_;
		TextToNodeMap* map_{};
		int currentLine_{};
		int currentColumn_{};
		QString renderedFile_;
		QList<Model::Node*> nodeStack_;

		Model::Node* pendingNodeToMap_{};
		Span pendingSpanToMap_{0, 0, 0, 0};
		void mapUntil(int endLine, int endColumn);
		void flushPending();


		void write(const QString& str);
		void writeLine(const QString& str = QString());
		void render(SourceFragment* fragment, QString indentationSoFar);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FragmentLayouter::IndentationFlags)

inline void FragmentLayouter::addRule(const QString& fragmentType, IndentationFlags parameters)
{
	Q_ASSERT(!rules_.contains(fragmentType));
	rules_.insert(fragmentType, parameters);
}

} /* namespace Export */
