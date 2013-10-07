/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "CommentDiagram.h"
#include "CommentDiagramShape.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
#include "VisualizationBase/src/items/Item.h"
DEFINE_TYPED_LIST(Comments::CommentDiagramShape)

namespace Comments {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommentDiagramShape)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommentDiagramShape)

REGISTER_ATTRIBUTE(CommentDiagramShape, label, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, x, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, y, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, width, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, height, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, shapeType, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, shapeColor, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, textColor, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, backgroundColor, Text, false, false, true)

// references for primitive types?
CommentDiagramShape::CommentDiagramShape(const int& x, const int& y, const int& width,
		const int& height, enum CommentDiagramShapeType shapeType)
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
}

QSize CommentDiagramShape::size() const
{
	return QSize(width(), height());
}

QPoint CommentDiagramShape::pos() const
{
	return QPoint(x(), y());
}

int CommentDiagramShape::hitsConnectorPoint(QPoint pos) const
{
	int index = -1;
	// max manhattan distance + 1 (area around the point to accept as a hit)
	int manhattan = 15;

	for(int i = 0; i < 16; ++i)
	{
		auto point = connectorPoints_[i];
		QPoint diff = point - pos;
		int m = diff.manhattanLength();
		if(m < manhattan)
		{
			manhattan = m;
			index = i;
		}
	}

	return index;
}

void CommentDiagramShape::updateConnectorPoints()
{
	switch(shapeType())
	{
	default:
	case Rectangle:
		for(int i = 0; i < 16; ++i)
		{
			// index 0 is upper left corner, normalize to compass directions (0 = top center = north)
			int index = (i + 2) % 16;

			if     (index >=  0 && index <  4) connectorPoints_[i] = QPoint(index/4.*width(), 0);
			else if(index >=  4 && index <  8) connectorPoints_[i] = QPoint(width(), (index-4)/4.*height());
			else if(index >=  8 && index < 12) connectorPoints_[i] = QPoint((12-index)/4.*width(), height());
			else if(index >= 12 && index < 16) connectorPoints_[i] = QPoint(0, (16-index)/4.*height());
		}
		return;

	case Ellipse:
		for(int i = 0; i < 16; ++i)
		{
			// index 0 is at the right center, normalize to compass directions
			int index = (i + 12) % 16;

			// based on mathematical derivation on
			// http://math.stackexchange.com/questions/22064/calculating-a-point-that-lies-on-an-ellipse-given-an-angle
			double a = width()/2., b = height()/2.;
			QPointF center(a, b);
			// distribute connector points equally over angles (0, pi/8, , ..., 15pi/8)
			// TODO: Make these equidistant?
			double angle = index*2*M_PI/16;
			double t = tan(angle);
			double x = a*b/(sqrt(b*b+a*a*t*t));
			// invert sign of result for angles between pi/2 and 3pi/2
			if(index > 4 && index < 12)
				x = -x;

			double y = t*x;

			// special cases where angle is pi/2 or 3pi/2
			if(index == 4)  y =  b;
			if(index == 12) y = -b;

			connectorPoints_[i] = QPoint(center.x()+x, center.y()+y);
		}
		return;

	case Diamond:
		for(int i = 0; i < 16; ++i)
		{
			// index 0 is already in the top center (north)
			//  => no compass normalization needed!
			if(i >=  0 && i <  4) connectorPoints_[i] = QPoint((i+4)/8.*width(), i/8.*height());
			if(i >=  4 && i <  8) connectorPoints_[i] = QPoint((8-(i-4))/8.*width(), (i/8.*height()));
			if(i >=  8 && i < 12) connectorPoints_[i] = QPoint((4-(i-8))/8.*width(), (8-(i-8))/8.*height());
			if(i >= 12 && i < 16) connectorPoints_[i] = QPoint((i-12)/8.*width(), (4-(i-12))/8.*height());
		}
		return;
	}
}

int CommentDiagramShape::index() const
{
	auto diagram = dynamic_cast<CommentDiagram*>(parent()->parent());
	Q_ASSERT(diagram != nullptr);
	return diagram->shapes()->indexOf(this);
}

QDebug operator<<(QDebug dbg, CommentDiagramShape *c)
{
	QString shapeType;

	if(c->shapeType() == Ellipse) shapeType = "Ellipse";
	else if(c->shapeType() == Diamond) shapeType = "Diamond";
	else shapeType = "Rectangle";

   dbg.nospace() << "(CommentDiagramShape: " << shapeType << " at (" << c->x() << "," << c->y()
   		         << ") size " << c->width() << "x" << c->height()
   		         << ", shapeColor " << c->shapeColor()
   		         << ", textColor " << c->textColor()
   		         << ", backgroundColor " << c->backgroundColor()
   		         << ", label " << c->label() << ")";
   return dbg.space();
}

} /* namespace Comments */
