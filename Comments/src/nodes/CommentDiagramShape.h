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

#pragma once

#include "../comments_api.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/Integer.h"
#include "ModelBase/src/nodes/TypedList.h"
#include "CommentText.h"

namespace Comments { class CommentDiagramShape; }
extern template class COMMENTS_API Model::TypedList<Comments::CommentDiagramShape>;

namespace Comments {

class COMMENTS_API CommentDiagramShape : public Super<Model::CompositeNode>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(CommentDiagramShape)

	ATTRIBUTE(Comments::CommentText, label, setLabel)
	ATTRIBUTE_VALUE_CUSTOM_RETURN(Model::Text, shapeColor, setShapeColor, QString, const QString&)
	ATTRIBUTE_VALUE_CUSTOM_RETURN(Model::Text, textColor, setTextColor, QString, const QString&)
	ATTRIBUTE_VALUE_CUSTOM_RETURN(Model::Text, backgroundColor, setBackgroundColor, QString, const QString&)
	ATTRIBUTE_VALUE(Model::Integer, outlineSize, setOutlineSize, int)
	ATTRIBUTE_VALUE(Model::Integer, x, setX, int)
	ATTRIBUTE_VALUE(Model::Integer, y, setY, int)
	ATTRIBUTE_VALUE(Model::Integer, width, setWidth, int)
	ATTRIBUTE_VALUE(Model::Integer, height, setHeight, int)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, shapeTypePrivate, setShapeTypePrivate, int)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, outlineTypeStore, setOutlineTypeStore, int)

	public:
		enum class ShapeType : int { Rectangle, Ellipse, Diamond };

		CommentDiagramShape(int x, int y, int width, int height, ShapeType shapeType);

		ShapeType shapeType() const;
		void setShapeType(const ShapeType& type);
		QSize size() const;
		QPoint pos() const;

		int index() const;

		QPoint connectorPoint(int index) const;
		int connectorPointNear(QPoint pos) const;

		Qt::PenStyle outlineType() const;
		void setOutlineType(const Qt::PenStyle& outlineType);

	private:
		mutable int lastRevisionForConnectors_ = {revision() -1};
		mutable QVector<QPoint> connectorPoints_;

		void reCalculateConnectorPoints() const;
		void assureConnectorPointsUpToDate() const;
};

inline CommentDiagramShape::ShapeType CommentDiagramShape::shapeType() const
{ return static_cast<ShapeType> (shapeTypePrivate()); }
inline void CommentDiagramShape::setShapeType(const ShapeType &type)
{ setShapeTypePrivate(static_cast<int> (type)); }

inline QSize CommentDiagramShape::size() const { return QSize{width(), height()}; }
inline QPoint CommentDiagramShape::pos() const { return QPoint{x(), y()}; }
inline void CommentDiagramShape::assureConnectorPointsUpToDate() const
{
	if (lastRevisionForConnectors_ != revision())
	{
		reCalculateConnectorPoints();
		lastRevisionForConnectors_ = revision();
	}
}

inline QPoint CommentDiagramShape::connectorPoint(int index) const
{ assureConnectorPointsUpToDate(); return connectorPoints_.at(index); }

inline Qt::PenStyle CommentDiagramShape::outlineType() const { return static_cast<Qt::PenStyle> (outlineTypeStore()); }
inline void CommentDiagramShape::setOutlineType(const Qt::PenStyle& outlineType) { setOutlineTypeStore(outlineType); }

}
