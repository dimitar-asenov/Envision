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

class EXPORT_API FileWriter {
	public:
		FileWriter(const QString& fileName, TextToNodeMap* map);

		bool isAtStartOfLine() const;
		bool lastCharacterIsWhiteSpace() const;
		void write(const QString& str);
		void finishLine();
		void writeEmptyLine();

		void appendNodeToStack(Model::Node* node);
		void popLastNodeFromStack();

		/**
		 * Flushes any remaining writes, appends an empty line to the end of the file, and returns the entire file text.
		 */
		QString fileContents();

		/**
		 * returns the amount of bytes written to the buffer of this writer so far.
		 * used to add backslashes to all lines of the buffer after this value when exporting C++ macros.
		 */
		int bytesWrittenSoFar();

		/**
		 * adds backslashes to all lines of the part of the buffer after \a bytes bytes.
		 */
		void appendBackslashToLinesAfter(int bytes);

	private:
		QString fileName_;
		TextToNodeMap* map_{};
		int currentLine_{};
		int currentColumn_{};
		QString renderedFile_;
		QList<Model::Node*> nodeStack_;

		Model::Node* pendingNodeToMap_{};
		Span pendingSpanToMap_{0, 0, 0, 0};

		void flushPending();
		void mapUntil(int endLine, int endColumn);
};

inline bool FileWriter::isAtStartOfLine() const { return currentColumn_ == 0;}
inline bool FileWriter::lastCharacterIsWhiteSpace() const
{ return renderedFile_.isEmpty() || renderedFile_.endsWith('\n')
			|| renderedFile_.endsWith('\t') || renderedFile_.endsWith(' '); }
inline void FileWriter::appendNodeToStack(Model::Node* node) { nodeStack_.append(node); }
inline void FileWriter::popLastNodeFromStack() { nodeStack_.removeLast(); }

} /* namespace Export */
