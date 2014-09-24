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

#include "CompilerOutputParser.h"

namespace OODebug {

void CompilerOutputParser::parseJavacErrorFormat(const QString& output)
{
	clearFeedback();

	/**
	 * Javac Error format:
	 *
	 * The first group matches the filename
	 * The second group matches the lineNumber
	 * The third group matches the type (e.g. error, warning, etc.)
	 * The fourth group matches the message
	 * The fifth group matches the spaces before the ^ which gives the column.
	 *
	 * To edit you may want to use this: http://regex101.com/r/cX9lM8/2
	 */
	QRegularExpression feedbackReg("(^[a-zA-Z0-9_\\/]+\\.java):([0-9]+): ([a-z]+): ([^\\n]+)\\n[^\\n]+([^\\^]+)",
				QRegularExpression::MultilineOption);

	auto matches = feedbackReg.globalMatch(output);
	while (matches.hasNext()) {
		auto currentMatch = matches.next();
		QString file = currentMatch.captured(1);
		int lineNumber = currentMatch.captured(2).toInt();
		QString type = currentMatch.captured(3);
		QString msg = currentMatch.captured(4);
		int colNumber = currentMatch.captured(5).length();

		if (!type.compare("error")) {
			errors_.append(CompilerFeedback(CompilerFeedback::Error, file, msg, lineNumber, colNumber));
		} else if (!type.compare("warning")) {
			warnings_.append(CompilerFeedback(CompilerFeedback::Warning, file, msg, lineNumber, colNumber));
		} else if (!type.compare("note")) {
			notes_.append(CompilerFeedback(CompilerFeedback::Note, file, msg, lineNumber, colNumber));
		} else {
			qWarning() << "Not supported feedback type:" << type;
			others_.append(CompilerFeedback(CompilerFeedback::Other, file, msg, lineNumber, colNumber));
		}
	}
}

void CompilerOutputParser::clearFeedback()
{
	errors_.clear();
	warnings_.clear();
	notes_.clear();
	others_.clear();
}

} /* namespace OODebug */
