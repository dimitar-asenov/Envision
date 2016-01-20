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

void FragmentLayouter::addRule(const QString& fragmentType, IndentationFlags parameters, const QString& prefix,
										 const QString& separator, const QString& postfix)
{
	Q_ASSERT(!rules_.contains(fragmentType));
	rules_.insert(fragmentType, qMakePair(parameters, FragmentDecoration{prefix, separator, postfix}));
}

QString FragmentLayouter::render(SourceFile* file, TextToNodeMap* map)
{
	Q_ASSERT(file);

	writer_ = new FileWriter{file->path(), map};
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
		if (text->text() == "\n")
			writer_->writeEmptyLine();
		else
		{
			if (!text->text().isEmpty() && writer_->isAtStartOfLine()) writer_->write(indentationSoFar);
			writer_->write(text->text());
		}
		return;
	}

	auto composite = dynamic_cast<CompositeFragment*>(fragment);
	Q_ASSERT(composite);

	auto rule = rules_.value(composite->type());

	// Indent first line
	if (rule.first.testFlag(NewLineBefore)) writer_->finishLine();
	if (!rule.second.prefix_.isEmpty() && writer_->isAtStartOfLine()) writer_->write(indentationSoFar);

	// Prefix
	if (rule.first.testFlag(IndentPrePostFix)) writer_->write(indentation_);
	writer_->write(rule.second.prefix_);
	if (rule.first.testFlag(SpaceAfterPrefix)) writer_->write(" ");
	if (rule.first.testFlag(NewLineAfterPrefix)) writer_->finishLine();

	// Child fragments
	auto firstSubFragment = true;
	QString childFragmentIndentation = indentationSoFar
			+ (rule.first.testFlag(IndentChildFragments) ? indentation_ : "");
	int bytesWrittenSoFar = writer_->bytesWrittenSoFar();
	for ( auto subFragment : composite->fragments())
	{
		// Separator, after first child
		if (!firstSubFragment)
		{
			if (rule.first.testFlag(SpaceBeforeSeparator)) writer_->write(" ");
			if ( rule.second.separator_ == "\n") writer_->finishLine();
			else if ( rule.second.separator_ == "\n\n") writer_->writeEmptyLine();
			else writer_->write(rule.second.separator_);
			if (rule.first.testFlag(SpaceAfterSeparator)) writer_->write(" ");
		}

		// This child fragment
		render(subFragment, childFragmentIndentation);
		firstSubFragment = false;
	}

	// Postfix
	if (rule.first.testFlag(NewLineBeforePostfix)) writer_->finishLine();
	if (!rule.second.postfix_.isEmpty() && writer_->isAtStartOfLine()) writer_->write(indentationSoFar);
	if (rule.first.testFlag(IndentPrePostFix)) writer_->write(indentation_);
	if (rule.first.testFlag(SpaceBeforePostfix)) writer_->write(" ");
	writer_->write(rule.second.postfix_);
	if (rule.first.testFlag(SpaceAtEnd) && !writer_->lastCharacterIsWhiteSpace()) writer_->write(" ");
	if (rule.first.testFlag(NewLineAfterPostfix)) writer_->finishLine();
	if (rule.first.testFlag(EmptyLineAtEnd)) writer_->writeEmptyLine();
	if (rule.first.testFlag(BackslashAfterLines)) writer_->appendBackslashToLinesAfter(bytesWrittenSoFar);
}

}
