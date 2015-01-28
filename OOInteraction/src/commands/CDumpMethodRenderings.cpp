/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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
#include "CDumpMethodRenderings.h"

#include "CommandHelper.h"

using namespace Interaction;
using namespace Visualization;
using namespace Model;

namespace OOInteraction {

CDumpMethodRenderings::CDumpMethodRenderings() : CommandWithNameAndFlags("dump-method-renderings", {}, false)
{}

Interaction::CommandResult* CDumpMethodRenderings::executeNamed(Visualization::Item*,
		Visualization::Item* target, const std::unique_ptr<Visualization::Cursor>&,
		const QString& name, const QStringList&)
{
	QStringList methods;

	// Read method signatures
	{
		QFile methodSpecificationFile{name};
		if (!methodSpecificationFile.exists())
			return new CommandResult( new CommandError("Could not find file " + name));

		if (!methodSpecificationFile.open(QIODevice::ReadOnly | QIODevice::Text))
			return new CommandResult( new CommandError("Could not open file " + name));

		QTextStream textStream(&methodSpecificationFile);

		const int MAX_NUM_METHODS = 150;

		for (int i = 0; i<MAX_NUM_METHODS; ++i)
		{
			QString line = textStream.readLine();
			if (line.isNull()) break;
			else methods.append(line);
		}
	}

	QRegExp reg{"^.+(\\.\\S+)\\s+>>>.*\\s(\\S+)\\(.*\\).*$"};
	int itemNumber = 1;
	for (auto methodSig : methods)
	{
		bool matched = reg.exactMatch(methodSig);
		if (!matched)
			return new CommandResult( new CommandError("Format mismatch on line: " + methodSig));

		QStringList path {reg.capturedTexts()[1].split(".", QString::SkipEmptyParts)};
		path += reg.capturedTexts()[2];

		int numPrinted = 0;
		printPath(target, path, itemNumber, numPrinted);
		++itemNumber;

		if (numPrinted == 0)
			return new CommandResult( new CommandError("Target path not found: " + path.join(".")));
	}

	return new CommandResult();
}

void CDumpMethodRenderings::printPath(Visualization::Item* item, QStringList path, int itemNumber, int& numPrinted)
{
	Q_ASSERT(!path.isEmpty());
	Q_ASSERT(item->hasNode());

	if (item->node()->symbolName() != path.takeFirst()) return;

	if (path.isEmpty()) printItem(item, itemNumber, numPrinted);
	else
	{
		QSet<Node*> nextInPath;
		item->node()->findSymbols(nextInPath, path.first(), item->node(), Node::SEARCH_DOWN, Node::ANY_SYMBOL, false);
		for (auto next : nextInPath)
			if (auto itemForNextNode = item->findVisualizationOf(next))
				printPath(itemForNextNode, path, itemNumber, numPrinted);
	}
}

void CDumpMethodRenderings::printItem(Visualization::Item* item, int itemNumber, int& numPrinted)
{
	++numPrinted;

	QSize itemSize = item->sizeInScene().toSize();
	QImage image(itemSize, QImage::Format_RGB32);
	image.fill(Qt::white);
	QPainter pmapPainter(&image);
	pmapPainter.setRenderHint(QPainter::Antialiasing);
	item->scene()->render(&pmapPainter, QRect(0, 0, itemSize.width(), itemSize.height()),
								 item->sceneBoundingRect().toRect());
	image.save("item_dump/" + QString::number(itemNumber) + "_" + QString::number(numPrinted) + ".png");
}

} /* namespace OOInteraction */
