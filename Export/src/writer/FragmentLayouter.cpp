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
#include "FragmentLayouter.h"
#include "TextToNodeMap.h"
#include "../tree/SourceFile.h"
#include "../tree/TextFragment.h"
#include "../tree/CompositeFragment.h"

namespace Export {

FragmentLayouter::FragmentLayouter(const QString& indentation)
	:indentation_{indentation}
{}

QString FragmentLayouter::render(SourceFile* file, TextToNodeMap* map)
{
	Q_ASSERT(file);
	Q_ASSERT(map);

	fileName_ = file->path();
	map_ = map;
	currentLine_ = 0;
	currentColumn_ = 0;

	for (auto fragment : file->fragments())
		render(fragment, "");
	flushPending();

	map_ = nullptr;
	fileName_.clear();
	currentLine_ = 0;
	currentColumn_ = 0;

	return renderedFile_;
}

void FragmentLayouter::render(SourceFragment* fragment, QString indentationSoFar)
{
	nodeStack_.append(fragment->node());
	OnScopeExit removeLastNode{[this](){nodeStack_.removeLast();}};

	if (auto text = dynamic_cast<TextFragment*>(fragment))
	{
		write(text->text());
		return;
	}

	auto composite = dynamic_cast<CompositeFragment*>(fragment);
	Q_ASSERT(composite);

	// Indent first line
	if (currentColumn_ == 0) write(indentationSoFar);

	auto rules = rules_.value(composite->type());

	// Prefix
	if (rules.testFlag(IndentPrePostFix)) write(indentation_);
	write(composite->prefix());
	if (rules.testFlag(SpaceAfterPrefix)) write(" ");
	if (rules.testFlag(NewLineAfterPrefix)) writeLine();

	// Child fragments
	auto firstSubFragment = true;
	QString childFragmentIndentation = indentationSoFar + (rules.testFlag(IndentChildFragments) ? indentation_ : "");
	for ( auto subFragment : composite->fragments())
	{
		// Separator, after first child
		if (!firstSubFragment)
		{
			if (rules.testFlag(SpaceBeforeSeparator)) write(" ");
			if ( composite->separator() == "\n") writeLine();
			else write(composite->separator());
			if (rules.testFlag(SpaceAfterSeparator)) write(" ");
		}

		// This child fragment
		render(subFragment, childFragmentIndentation);
		firstSubFragment = false;
	}

	// Postfix
	if (rules.testFlag(NewLineBeforePostfix) && currentColumn_ != 0) writeLine();
	if (currentColumn_ == 0) write(indentationSoFar);
	if (rules.testFlag(IndentPrePostFix)) write(indentation_);
	if (rules.testFlag(SpaceBeforePostfix)) write(" ");
	write(composite->postfix());
	if (rules.testFlag(NewLineAfterPostfix)) writeLine();
	if (rules.testFlag(EmptyLineAtEnd)) writeLine();
}

void FragmentLayouter::mapUntil(int endLine, int endColumn)
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

void FragmentLayouter::flushPending()
{
	if (pendingNodeToMap_)
	{
		map_->add(pendingNodeToMap_, {fileName_, pendingSpanToMap_});
		pendingNodeToMap_ = nullptr;
	}
}

void FragmentLayouter::write(const QString& str)
{
	if (str.isEmpty()) return;
	mapUntil(currentLine_, currentColumn_ + str.size());
	currentColumn_ += str.size();
	renderedFile_ += str;
}


void FragmentLayouter::writeLine(const QString& str)
{
	write(str);
	currentLine_++;
	currentColumn_ = 0;
}

} /* namespace Export */
