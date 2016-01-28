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

namespace Export {

class SourceFile;
class SourceFragment;
class FileWriter;
class TextToNodeMap;

struct EXPORT_API FragmentDecoration
{
	QString prefix_;
	QString separator_;
	QString postfix_;
};

class EXPORT_API FragmentLayouter {
	public:
		FragmentLayouter(const QString& indentation);

		enum IndentationFlag {
			NoIndentation = 0x0,
			NewLineBefore = 0x1,
			IndentPrePostFix = 0x2,
			IndentChildFragments = 0x4,
			SpaceAfterPrefix = 0x8,
			NewLineAfterPrefix = 0x10,
			SpaceBeforePostfix = 0x20,
			NewLineBeforePostfix = 0x40,
			NewLineAfterPostfix = 0x80,
			SpaceBeforeSeparator = 0x100,
			SpaceAfterSeparator = 0x200,
			EmptyLineAtEnd = 0x400,
			SpaceAtEnd = 0x800,
			BackslashAfterLines = 0x1000
		};
		using IndentationFlags = QFlags<IndentationFlag>;

		void addRule(const QString& fragmentType, IndentationFlags parameters, const QString& prefix = QString(),
						 const QString& separator = QString(), const QString& postfix = QString());

		QString render(SourceFile* file, TextToNodeMap* map);

	private:
		QString indentation_;
		QHash<QString, QPair<IndentationFlags, FragmentDecoration>> rules_;

		FileWriter* writer_{}; // Only used while renderering

		void render(SourceFragment* fragment, QString indentationSoFar);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FragmentLayouter::IndentationFlags)

}
