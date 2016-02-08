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

#include "GridLayouter.h"
#include "../cursor/LayoutCursor.h"
#include "../node_extensions/Position.h"
#include "ModelBase/src/nodes/composite/CompositeNode.h"

namespace Visualization {

ItemRegion GridLayouter::cursorRegion(Item* parent, FormElement* formElement, int xIndex, int yIndex,
		bool horizontal, bool atBoundary, bool notLocationEquivalent, bool mayExpandFront, bool mayExpandBack,
		QRect area)
{
	// Make sure there is at least some space for the cursor Region.
	if (horizontal && area.height() == 0 ) area.adjust(0, (mayExpandFront?-1:0), 0, (mayExpandBack?1:0));
	if (!horizontal && area.width() == 0) area.adjust((mayExpandFront?-1:0), 0, (mayExpandBack?1:0), 0);

	auto lc = new LayoutCursor{parent, horizontal ? Cursor::HorizontalCursor : Cursor::VerticalCursor};
	lc->setOwnerElement(formElement);
	lc->set2DIndex(xIndex, yIndex);
	lc->setIndex( horizontal ? yIndex : xIndex); // Works for 1D sequences
	lc->setVisualizationSize( area.size() );
	lc->setVisualizationPosition( area.topLeft() );
	lc->setRegion( area );
	lc->setIsAtBoundary(atBoundary);
	lc->setNotLocationEquivalent(notLocationEquivalent);

	auto region = ItemRegion{area};
	region.setCursor(lc);
	return region;
}

template<typename Container, typename Value>
inline void GridLayouter::resizeReplace(Container& container, int majorIndex, int minorIndex, Value value)
{
	if (container.size() <= majorIndex) container.resize(majorIndex+1);
	if (container[majorIndex].size() <= minorIndex) container[majorIndex].resize(minorIndex+1);
	container[majorIndex][minorIndex] = value;
}

QVector< QVector<Model::Node*>> GridLayouter::arrange(QVector<Model::Node*> nodes, MajorAxis majorAxis)
{
	QVector< QVector<Model::Node*>> result;
	QVector<Model::Node*> nodesWithoutPosition;
	// Compute width and height
	for (auto node : nodes)
	{
		auto composite = DCast<Model::CompositeNode>(node);
		Q_ASSERT(composite);
		auto position = composite->extension<Position>();
		Q_ASSERT(position);

		if ( position->xNode() && position->yNode() )
		{
			if (majorAxis == ColumnMajor)
				resizeReplace(result, position->x(), position->y(), composite);
			else
				resizeReplace(result, position->y(), position->x(), composite);
		}
		else nodesWithoutPosition.append(node);
	}

	int finalMajor = result.size();
	int minorIndex = 0;
	for (auto node : nodesWithoutPosition)
		resizeReplace(result, finalMajor, minorIndex++, node);

	// If using a major axis, make sure not to include nullptr nodes.
	if (majorAxis != NoMajor)
		for (auto& minorVector : result)
			for (int i = minorVector.size() - 1; i>=0; --i)
				if (minorVector[i] == nullptr)
					minorVector.remove(i);

	return result;
}

void GridLayouter::setPositionInGrid(QVector<Model::Node*> nodes, int x, int y, Model::Node* node, MajorAxis majorAxis)
{
	if (nodes.contains(node)) removeFromGrid(nodes, node, majorAxis);
	else normalizeGridIndices(nodes, majorAxis); // Otherwise this is called from removeFromGrid()

	pushNodes(nodes, x, y, 1, majorAxis, x==-1 || y==-1);

	auto composite = DCast<Model::CompositeNode>(node);
	Q_ASSERT(composite);
	auto position = composite->extension<Position>();
	Q_ASSERT(position);
	position->set(std::max(x, 0), std::max(y, 0));
}

void GridLayouter::removeFromGrid(QVector<Model::Node*> nodes, Model::Node* node, MajorAxis majorAxis)
{
	normalizeGridIndices(nodes, majorAxis);

	auto composite = DCast<Model::CompositeNode>(node);
	Q_ASSERT(composite);
	auto position = composite->extension<Position>();
	Q_ASSERT(position);
	if ( position->xNode()  == nullptr && position->yNode() == nullptr) return;

	pushNodes(nodes, position->x(), position->y(), -1, majorAxis, false);

	position->setXNode(nullptr);
	position->setYNode(nullptr);

}

void GridLayouter::pushNodes(QVector<Model::Node*> nodes, int x, int y, int pushAmount, MajorAxis majorAxis,
		bool pushAllMajor)
{
	for (auto existingNode : nodes)
	{
		auto composite = DCast<Model::CompositeNode>(existingNode);
		Q_ASSERT(composite);
		auto position = composite->extension<Position>();
		Q_ASSERT(position);

		if ( position->xNode() && position->yNode() )
		{
			if (pushAllMajor)
			{
				if (majorAxis == ColumnMajor && position->x() >= x)
					position->setX(position->x() + pushAmount);
				if (majorAxis != ColumnMajor && position->y() >= y)
					position->setY(position->y() + pushAmount);
			}
			else
			{
				if (majorAxis == ColumnMajor && x == position->x() && y<= position->y())
					position->setY(position->y() + pushAmount);
				if (majorAxis != ColumnMajor && y == position->y() && x<= position->x())
					position->setX(position->x() + pushAmount);
			}
		}
	}
}

void GridLayouter::normalizeGridIndices(QVector<Model::Node*> nodes, MajorAxis majorAxis)
{
	if (majorAxis == NoMajor) return;
	auto normalized = arrange(nodes, majorAxis);
	for (int major = 0; major < normalized.size(); ++major)
		for (int minor = 0; minor < normalized[major].size(); ++minor)
		{
			auto composite = DCast<Model::CompositeNode>(normalized[major][minor]);
			Q_ASSERT(composite);
			auto position = composite->extension<Position>();
			Q_ASSERT(position);

			int normalizedX = majorAxis == ColumnMajor ? major : minor;
			int normalizedY = majorAxis == ColumnMajor ? minor : major;
			if ( !position->xNode() || !position->yNode()||
					position->x() != normalizedX || position->y() != normalizedY)
				position->set(normalizedX, normalizedY);
		}
}

}
