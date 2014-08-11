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
#include "FileWriter.h"

namespace Export {

FileWriter::FileWriter(const QString& fileName, TextToNodeMap* map)
	: fileName_{fileName}, map_{map}
{
	Q_ASSERT(map);
}

void FileWriter::mapUntil(int endLine, int endColumn)
{
	if (nodeStack_.last() != pendingNodeToMap_)
	{
		flushPending();
		pendingNodeToMap_ = nodeStack_.last();
		pendingSpanToMap_.startLine_  = currentLine_;
		pendingSpanToMap_.startColumn_ = currentColumn_;
	}

	pendingSpanToMap_.endLine_ = endLine;
	pendingSpanToMap_.endColumn_ = endColumn;
}

void FileWriter::flushPending()
{
	if (pendingNodeToMap_)
	{
		map_->add(pendingNodeToMap_, {fileName_, pendingSpanToMap_});
		pendingNodeToMap_ = nullptr;
	}
}

void FileWriter::write(const QString& str)
{
	if (str.isEmpty()) return;
	mapUntil(currentLine_, currentColumn_ + str.size() - 1);
	currentColumn_ += str.size();
	renderedFile_ += str;
}


void FileWriter::writeLine(const QString& str)
{
	write(str + "\n");
	flushPending(); // do not span mapping across lines.
	currentLine_++;
	currentColumn_ = 0;
}


QString FileWriter::fileContents()
{
	flushPending();
	if (!isAtStartOfLine()) renderedFile_.append("\n");
	return renderedFile_;
}

} /* namespace Export */
