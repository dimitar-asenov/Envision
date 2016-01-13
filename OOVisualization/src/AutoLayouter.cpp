/***********************************************************************************************************************
**
** Copyright (c) 2011, 2016 ETH Zurich
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

#include "AutoLayouter.h"

#include "VisualizationBase/src/items/Item.h"

namespace OOVisualization {

AutoLayouter::Size AutoLayouter::estimateSize(Model::Node* node)
{
	if (auto reference = DCast<OOModel::OOReference>(node))
		return {reference->name().size()*TEXT_WIDTH, TEXT_HEIGHT};

	if (DCast<OOModel::NullLiteral>(node)
		 || DCast<OOModel::BreakStatement>(node)
		 || DCast<OOModel::ContinueStatement>(node))
		return {10*TEXT_WIDTH, TEXT_HEIGHT};

	if (DCast<OOModel::EmptyExpression>(node))
		return {TEXT_WIDTH, TEXT_HEIGHT};

	if (auto n = DCast<OOModel::BooleanLiteral>(node)) return estimatedSize(n->valueNode());
	if (auto n = DCast<OOModel::CharacterLiteral>(node)) return estimatedSize(n->valueNode());
	if (auto n = DCast<OOModel::FloatLiteral>(node)) return estimatedSize(n->valueNode());
	if (auto n = DCast<OOModel::IntegerLiteral>(node)) return estimatedSize(n->valueNode());
	if (auto n = DCast<OOModel::BooleanLiteral>(node)) return estimatedSize(n->valueNode());
	if (auto n = DCast<OOModel::StringLiteral>(node)) return estimatedSize(n->valueNode());

	if (auto n = DCast<OOModel::LoopStatement>(node)) return arrangeWithBody(n, n->body());
	if (auto n = DCast<OOModel::ForEachStatement>(node)) return arrangeWithBody(n, n->body());
	if (auto n = DCast<OOModel::SwitchStatement>(node)) return arrangeWithBody(n, n->body());
	if (auto n = DCast<OOModel::CaseStatement>(node)) return arrangeWithBody(n, n->body());

	if (auto n = DCast<OOModel::IfStatement>(node)) return arrangeIf(n);

	if (DCast<OOModel::Expression>(node)
		 || node->typeName().endsWith("ArrayInitializer")
		 || node->typeName().endsWith("Operation")
		 || DCast<OOModel::FormalResult>(node))
		return arrange(Direction::Horizontally, 0, node->children());
	if (DCast<OOModel::CatchClause>(node)
		 || DCast<OOModel::StatementItemList>(node)
		 || node->typeName() == "TypedListOfCatchClause"
		 || (DCast<OOModel::Statement>(node) && !node->typeName().startsWith("TypedList"))
		 || DCast<OOModel::FormalArgument>(node)
		 || DCast<OOModel::FormalTypeArgument>(node))
		return arrange(Direction::Vertically, 1, node->children());
	if (DCast<OOModel::NameImport>(node)
		 || DCast<OOModel::Field>(node)
		 || DCast<OOModel::VariableDeclaration>(node))
		return arrange(Direction::Horizontally, 0, node->children());
	if (auto n = DCast<OOModel::Method>(node)) return arrangeMethod(n);
	if (auto n = DCast<OOModel::Class>(node)) return arrangeClass(n);
	if (auto n = DCast<OOModel::Module>(node)) return arrangeModule(n);
	if (auto n = DCast<OOModel::Project>(node)) return arrangeProject(n);

	return arrange(Direction::Horizontally, 0, node->children());
}

void AutoLayouter::arrange(Model::Node* tree, Mode mode)
{
	estimatedSizeMap().clear();

	if (mode == Mode::Estimated)
		estimateSize(tree);
	else if (mode == Mode::Measured)
	{
		QList<Model::Node*> workStack{tree};
		while (!workStack.empty())
		{
			auto node = workStack.takeLast();

			if (!DCast<OOModel::Method>(node) && !DCast<OOModel::Class>(node) &&
				 !DCast<OOModel::Module>(node) && !DCast<OOModel::Project>(node))
			{
				auto it = Visualization::Item::nodeItemsMap().find(node);
				if (it != Visualization::Item::nodeItemsMap().end() && it.key() == node)
					estimatedSizeMap().insert(node, { (float)it.value()->widthInParent(),
																 (float)it.value()->heightInParent()});
				else
					estimatedSizeMap().insert(node, {0, 0});
			}
			workStack << node->children();
		}
		estimateSize(tree);
	}
	else
		Q_ASSERT(false);
}

AutoLayouter::Size AutoLayouter::arrange(AutoLayouter::Direction d, int spaceBetween, QList<Model::Node*> nodes)
{
	Size size{0, 0};
	bool passTheFirstNode = false;
	for (Model::Node* n : nodes)
	{
		if (d == Direction::Horizontally)
		{
			size.addHorizontally( n );
			if (passTheFirstNode) size.width += spaceBetween;
		}
		else
		{
			size.addVertically( n );
			if (passTheFirstNode) size.height += spaceBetween;
		}

		passTheFirstNode = true;
	}

	return size;
}

AutoLayouter::Size AutoLayouter::arrangeWithBody(Model::Node* nodeWithBody, Model::Node* body)
{
	Size size{24.0f, 24.0f};
	for (Model::Node* child : nodeWithBody->children())
		if (child != body)
			size.addHorizontally(child);

	size.addVertically(body);

	size.height += 20; // Compensation for empty space in header/footer
	return size;
}

AutoLayouter::Size AutoLayouter::arrangeIf(OOModel::IfStatement* ifs)
{
	Size size{24.0f, 24.0f};
	size.addHorizontally(ifs->condition());
	size.addVertically(ifs->thenBranch());
	size.addVertically(ifs->elseBranch());

	size.height += 20; // Compensation for empty space in header/footer
	return size;
}

AutoLayouter::Size AutoLayouter::arrangeMethod(OOModel::Method* me)
{
	Size size{24.0f, 24.0f};
	size.addHorizontally(me->nameNode());
	size.addHorizontally(me->typeArguments());
	size.addHorizontally(me->arguments());
	size.addHorizontally(me->results());

	size.addVertically(me->annotations());
	size.addVertically(me->subDeclarations());
	size.addVertically(me->memberInitializers());
	size.addVertically(me->items());

	size.height += 35; // Compensation for empty space, lines etc.
	return size;
}

AutoLayouter::Size AutoLayouter::arrangeClass(OOModel::Class* cl)
{
	Size size{24.0f, 24.0f};
	size.addHorizontally(cl->nameNode());
	size.addHorizontally(cl->baseClasses());
	size.addHorizontally(cl->typeArguments());

	size.addVertically(cl->annotations());
	size.addVertically(cl->subDeclarations());
	size.addVertically(cl->enumerators());
	size.addVertically(cl->friends());

	// Arrange body items
	QList<Model::Node*> bodyNodes;
	bodyNodes << cl->classes()->children();
	bodyNodes << cl->methods()->children();

	Size bodySize = arrangeNodesInGrid(bodyNodes, Major::ColumnMajor);
	size.addVertically(bodySize);

	size.height += 35; // Compensation for empty space, lines etc.
	return size;
}

AutoLayouter::Size AutoLayouter::arrangeModule(OOModel::Module* mo)
{
	Size size{24.0f, 24.0f};
	size.addHorizontally(mo->nameNode());
	size.addVertically(mo->annotations());
	size.addVertically(mo->subDeclarations());

	// Arrange body items
	QList<Model::Node*> bodyNodes;
	bodyNodes << mo->modules()->children();
	bodyNodes << mo->classes()->children();
	bodyNodes << mo->methods()->children();

	Size bodySize = arrangeNodesInGrid(bodyNodes, Major::ColumnMajor);
	size.addVertically(bodySize);

	size.height += 35; // Compensation for empty space, lines etc.
	return size;
}

AutoLayouter::Size AutoLayouter::arrangeProject(OOModel::Project* pr)
{
	Size size{24.0f, 24.0f};
	size.addHorizontally(pr->nameNode());
	size.addVertically(pr->annotations());
	size.addVertically(pr->subDeclarations());

	// Arrange body items
	QList<Model::Node*> bodyNodes;
	bodyNodes << pr->projects()->children();
	bodyNodes << pr->modules()->children();
	bodyNodes << pr->classes()->children();
	bodyNodes << pr->methods()->children();

	Size bodySize = arrangeNodesInGrid(bodyNodes, Major::ColumnMajor);
	size.addVertically(bodySize);

	size.height += 35; // Compensation for empty space, lines etc.
	return size;
}

QHash<Model::Node*, AutoLayouter::Size>& AutoLayouter::estimatedSizeMap()
{
	static QHash<Model::Node*, Size> estimatedSizes;
	return estimatedSizes;
}

AutoLayouter::Size AutoLayouter::estimatedSize(Model::Node* node)
{
	auto it = estimatedSizeMap().find(node);
	if (it == estimatedSizeMap().end())
		it = estimatedSizeMap().insert(node, estimateSize(node));
	return *it;
}

AutoLayouter::Size AutoLayouter::arrangeNodesInGrid(QList<Model::Node*> nodes, AutoLayouter::Major major)
{
	// Get average width/height
	float averageWidth = 0;
	float averageHeight = 0;
	for (Model::Node* n : nodes)
	{
		averageWidth += estimatedSize(n).width;
		averageHeight += estimatedSize(n).height;
	}
	averageWidth /= nodes.size();
	averageHeight /= nodes.size();

	// Get 'optimal' number of major/minor Elements to achieve a square
	float prevRatio = 0;
	int minorElements = 1;
	for (int majorElements = 1; majorElements<=nodes.size(); ++majorElements)
	{
		minorElements = (nodes.size()/majorElements) + ((nodes.size() % majorElements) > 0 ? 1:0);

		float ratio;
		if (major == Major::ColumnMajor)
			ratio = (averageWidth*majorElements) / (averageHeight*minorElements);
		else
			ratio = (averageWidth*minorElements) / (averageHeight*majorElements);

		if (ratio > 1) ratio = 1/ratio;

		if (ratio > prevRatio) prevRatio = ratio;
		else
		{
			if (majorElements > 1) --majorElements;
			minorElements = (nodes.size()/majorElements) + ((nodes.size() % majorElements) > 0 ? 1:0);
			break;
		}
	}

	float minorLengthLimit = minorElements*(major == Major::ColumnMajor ? averageHeight : averageWidth);

	int x = 0;
	int y = 0;
	Size finalSize{0, 0};
	float maxMajor = 0;
	float lengthSoFar = 0;

	for (Model::Node* child : nodes)
	{
		auto compositeNode = DCast<Model::CompositeNode>(child);
		Q_ASSERT(compositeNode);

		// Set the position of the item
		compositeNode->beginModification();
		std::unique_ptr<Visualization::Position>(compositeNode->extension<Visualization::Position>())->set(x, y);
		compositeNode->endModification();

		Size childSize = estimatedSize(child);
		if (major == Major::ColumnMajor)
		{
			lengthSoFar += childSize.height;
			if (childSize.width > maxMajor) maxMajor = childSize.width;

			if (lengthSoFar < minorLengthLimit) ++y;
			else
			{
				finalSize.width += maxMajor;
				maxMajor = 0;
				if (finalSize.height < lengthSoFar) finalSize.height = lengthSoFar;
				lengthSoFar = 0;

				y = 0;
				++x;
			}
		}
		else
		{
			lengthSoFar += childSize.width;
			if (childSize.height > maxMajor) maxMajor = childSize.height;

			if (lengthSoFar < minorLengthLimit) ++x;
			else
			{
				finalSize.height += maxMajor;
				maxMajor = 0;
				if (finalSize.width < lengthSoFar) finalSize.width = lengthSoFar;
				lengthSoFar = 0;

				x = 0;
				++y;
			}
		}
	}

	return finalSize;
}

}
