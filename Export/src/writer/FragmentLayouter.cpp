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

#include "FileWriter.h"
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

	writer_ = new FileWriter(file->path(), map);
	std::unique_ptr<FileWriter> deleter{writer_};

	for (auto fragment : file->fragments())
		render(fragment, "");

	return writer_->fileContents();
}

void FragmentLayouter::render(SourceFragment* fragment, QString indentationSoFar)
{
	writer_->appendNodeToStack(fragment->node());
	OnScopeExit removeLastNode{[this](){writer_->popLastNodeFromStack();}};

	if (auto text = dynamic_cast<TextFragment*>(fragment))
	{
		if (writer_->isAtStartOfLine()) writer_->write(indentationSoFar);
		writer_->write(text->text());
		return;
	}

	auto composite = dynamic_cast<CompositeFragment*>(fragment);
	Q_ASSERT(composite);

	auto rules = rules_.value(composite->type());

	// Indent first line
	if (rules.testFlag(NewLineBefore) && !writer_->isAtStartOfLine()) writer_->writeLine();
	if (writer_->isAtStartOfLine()) writer_->write(indentationSoFar);

	// Prefix
	if (rules.testFlag(IndentPrePostFix)) writer_->write(indentation_);
	writer_->write(composite->prefix());
	if (rules.testFlag(SpaceAfterPrefix)) writer_->write(" ");
	if (rules.testFlag(NewLineAfterPrefix)) writer_->writeLine();

	// Child fragments
	auto firstSubFragment = true;
	QString childFragmentIndentation = indentationSoFar + (rules.testFlag(IndentChildFragments) ? indentation_ : "");
	for ( auto subFragment : composite->fragments())
	{
		// Separator, after first child
		if (!firstSubFragment)
		{
			if (rules.testFlag(SpaceBeforeSeparator)) writer_->write(" ");
			if ( composite->separator() == "\n") writer_->writeLine();
			else writer_->write(composite->separator());
			if (rules.testFlag(SpaceAfterSeparator)) writer_->write(" ");
		}

		// This child fragment
		render(subFragment, childFragmentIndentation);
		firstSubFragment = false;
	}

	// Postfix
	if (rules.testFlag(NewLineBeforePostfix) && !writer_->isAtStartOfLine()) writer_->writeLine();
	if (writer_->isAtStartOfLine()) writer_->write(indentationSoFar);
	if (rules.testFlag(IndentPrePostFix)) writer_->write(indentation_);
	if (rules.testFlag(SpaceBeforePostfix)) writer_->write(" ");
	writer_->write(composite->postfix());
	if (rules.testFlag(NewLineAfterPostfix)) writer_->writeLine();
	if (rules.testFlag(EmptyLineAtEnd)) writer_->writeLine();
}

} /* namespace Export */
