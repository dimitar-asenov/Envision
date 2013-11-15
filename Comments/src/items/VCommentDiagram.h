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

#pragma once

#include "../comments_api.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/renderer/ModelRenderer.h"

#include "../nodes/CommentDiagram.h"
#include "../nodes/CommentNode.h"

namespace Comments {

class VCommentDiagramShape;
class VCommentDiagramConnector;

class COMMENTS_API VCommentDiagram : public Super<Visualization::ItemWithNode<VCommentDiagram,
						Visualization::Item, CommentDiagram> >
{
	ITEM_COMMON_CUSTOM_STYLENAME(VCommentDiagram, Visualization::ItemStyle)

	public:
		VCommentDiagram(Visualization::Item* parent, NodeType* node);
		void resize(QSize size);

		QPoint lastRightClick() const;
		void setLastRightClick(QPoint pos);

		bool editing() const;
		void toggleEditing();

		bool showConnectorPoints() const;
		void setShowConnectorPoints(bool show);

		QPair<int,int> lastConnector() const;
		void setLastConnector(int shape, int point);
		VCommentDiagramShape* diagramShape(int index);

	protected:
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	private:

		const static int EDIT_OUTLINE_SIZE = 10;

		QVector<VCommentDiagramShape*> shapes_;
		QVector<VCommentDiagramConnector*> connectors_;

		bool editing_{};
		bool showConnectorPoints_{};
		QPair<int,int> lastConnector_{-1, -1};
		QPoint lastRightClick_;

		template <class T>
		void synchronizeWithNodes(const QVector<Model::Node*>& nodes, QVector<T*>& destination);

		void clearChildren();
};

inline QPoint VCommentDiagram::lastRightClick() const { return lastRightClick_; }
inline void VCommentDiagram::setLastRightClick(QPoint pos) { lastRightClick_ = pos; }
inline bool VCommentDiagram::editing() const { return editing_; }
inline bool VCommentDiagram::showConnectorPoints() const { return editing() && showConnectorPoints_; }
inline QPair<int,int> VCommentDiagram::lastConnector() const { return lastConnector_; }
inline void VCommentDiagram::setLastConnector(int shape, int point) { lastConnector_ = qMakePair(shape, point); }
inline VCommentDiagramShape* VCommentDiagram::diagramShape(int index) { return shapes_.at(index);}

} /* namespace Comments */
