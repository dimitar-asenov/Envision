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

#include "CommentDiagramShape.h"
#include "CommentDiagram.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<Comments::CommentDiagramShape>;

namespace Comments {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommentDiagramShape)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommentDiagramShape)

REGISTER_ATTRIBUTE(CommentDiagramShape, label, CommentText, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, x, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, y, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, width, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, height, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, shapeTypePrivate, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, shapeColor, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, textColor, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, backgroundColor, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, outlineTypeStore, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, outlineSize, Integer, false, false, true)

CommentDiagramShape::CommentDiagramShape(int x, int y, int width, int height, ShapeType shapeType)
	: Super{nullptr, CommentDiagramShape::getMetaData()}
{
	setX(x);
	setY(y);
	setWidth(width);
	setHeight(height);
	setShapeType(shapeType);
	setShapeColor("black");
	setTextColor("black");
	setBackgroundColor("white");
	setOutlineTypeStore(1);
	setOutlineSize(1);
}

int CommentDiagramShape::index() const
{
	auto diagram = DCast<CommentDiagram>(parent()->parent());
	Q_ASSERT(diagram != nullptr);
	return diagram->shapes()->indexOf(this);
}

void CommentDiagramShape::reCalculateConnectorPoints() const
{
	connectorPoints_.clear();
	auto shapeWidth = width();
	auto shapeHeight = height();

	switch (shapeType())
	{
		case ShapeType::Rectangle:
			for (int i = 0; i < 16; ++i)
			{
				// index 0 is upper left corner, normalize to compass directions (0 = top center = north)
				int index = (i + 2) % 16;

				if (index >=  0 && index <  4)
					connectorPoints_.append(QPoint(index/4.*shapeWidth, 0));
				else if (index >=  4 && index <  8)
					connectorPoints_.append(QPoint(shapeWidth, (index-4)/4.*shapeHeight));
				else if (index >=  8 && index < 12)
					connectorPoints_.append(QPoint((12-index)/4.*shapeWidth, shapeHeight));
				else if (index >= 12 && index < 16)
					connectorPoints_.append(QPoint(0, (16-index)/4.*shapeHeight));
			}
			break;

		case ShapeType::Ellipse:
			for (int i = 0; i < 16; ++i)
			{
				// index 0 is at the right center, normalize to compass directions
				int index = (i + 12) % 16;

				// based on mathematical derivation on
				// http://math.stackexchange.com/questions/22064/calculating-a-point-that-lies-on-an-ellipse-given-an-angle
				double a = shapeWidth/2., b = shapeHeight/2.;
				QPointF center(a, b);
				// distribute connector points equally over angles (0, pi/8, , ..., 15pi/8)
				// TODO: Make these equidistant?
				double angle = index*2*M_PI/16;
				double t = tan(angle);
				double x = a*b/(sqrt(b*b+a*a*t*t));
				// invert sign of result for angles between pi/2 and 3pi/2
				if (index > 4 && index < 12)
					x = -x;

				double y = t*x;

				// special cases where angle is pi/2 or 3pi/2
				if (index == 4)  y =  b;
				if (index == 12) y = -b;

				connectorPoints_.append( QPoint(center.x()+x, center.y()+y));
			}
			break;

		case ShapeType::Diamond:
			for (int i = 0; i < 16; ++i)
			{
				// index 0 is already in the top center (north)
				//  => no compass normalization needed!
				if (i >=  0 && i <  4)
					connectorPoints_.append(QPoint((i+4)/8.*shapeWidth, i/8.*shapeHeight));
				if (i >=  4 && i <  8)
						connectorPoints_.append(QPoint((8-(i-4))/8.*shapeWidth, (i/8.*shapeHeight)));
				if (i >=  8 && i < 12)
					connectorPoints_.append(QPoint((4-(i-8))/8.*shapeWidth, (8-(i-8))/8.*shapeHeight));
				if (i >= 12 && i < 16)
					connectorPoints_.append(QPoint((i-12)/8.*shapeWidth, (4-(i-12))/8.*shapeHeight));
			}
			break;
		default: Q_ASSERT_X(false, "", "Unknown shape type");
	}
}

int CommentDiagramShape::connectorPointNear(QPoint pos) const
{
	assureConnectorPointsUpToDate();

	int index = -1;
	// max manhattan distance + 1 (area around the point to accept as a hit)
	int manhattan = 15;

	for (int i = 0; i < connectorPoints_.size(); ++i)
	{
		auto point = connectorPoints_[i];
		QPoint diff = point - pos;
		int m = diff.manhattanLength();
		if (m < manhattan)
		{
			manhattan = m;
			index = i;
		}
	}

	return index;
}

}
