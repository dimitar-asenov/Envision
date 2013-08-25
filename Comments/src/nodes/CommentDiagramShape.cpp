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

#include "CommentDiagramShape.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
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

// references for primitive types?
CommentDiagramShape::CommentDiagramShape(const QString& label, const int& x, const int& y, const int& width,
		const int& height, enum CommentDiagramShapeType shapeType)
: Super{nullptr, CommentDiagramShape::getMetaData()}
{
	setLabel(label);
	setX(x);
	setY(y);
	setWidth(width);
	setHeight(height);
	setShapeType(shapeType);
}

QSize CommentDiagramShape::size()
{
	return QSize(width(), height());
}

QPoint CommentDiagramShape::pos()
{
	return QPoint(x(), y());
}

QPoint CommentDiagramShape::getConnectorCoordinates(const int& index)
{
	Q_ASSERT(index >= 0 && index < 16);
//	qDebug() << "x,y" << x() << y() << "width,height" << width() << height() << "index" << index;
//	qDebug() << (height()-y());
	switch(shapeType())
	{
	default:
	case Rectangle:
		if     (index >=  0 && index <  4) return QPoint(x() + index/4.*width(), y());
		else if(index >=  4 && index <  8) return QPoint(x() + width(), y() + (index-4)/4.*height());
		else if(index >=  8 && index < 12) return QPoint(x() + (12-index)/4.*width(), y()+height());
		else if(index >= 12 && index < 16) return QPoint(x(), y()+(16-index)/4.*height());
		break;

	case Ellipse: {
		// based on mathematical derivation on
		// http://math.stackexchange.com/questions/22064/calculating-a-point-that-lies-on-an-ellipse-given-an-angle
		QPointF center(x()+width()/2., y()+height()/2.);
		double a = width()/2, b = height()/2;
		double angle = index*2*M_PI/16;
		double t = tan(angle);
		double x = a*b/(sqrt(b*b+a*a*t*t));
		// invert sign of result for angles between pi/2 and 3pi/2
		if(index > 4 && index < 12) {
			x = -x;
		}
		double y = t*x;

		// special cases where angle is pi/2 or 3pi/2
		if(index == 4)  y =  b;
		if(index == 12) y = -b;

		return QPoint(center.x()+x, center.y()+y);
	}

	case Diamond:
		if(index >=  0 && index <  4) return QPoint(x()+(index+4)/8.*width(), y()+index/8.*height());
		if(index >=  4 && index <  8) return QPoint(x()+(8-(index-4))/8.*width(), y()+(index/8.*height()));
		if(index >=  8 && index < 12) return QPoint(x()+(4-(index-8))/8.*width(), y()+(8-(index-8))/8.*height());
		if(index >= 12 && index < 16) return QPoint(x()+(index-12)/8.*width(), y()+(4-(index-12))/8.*height());
		break;
	}

	Q_ASSERT(!"Impossible");
	return QPoint();
}

} /* namespace Comments */
