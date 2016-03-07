/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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
#pragma once

#include "../visualizationbase_api.h"
#include "../nodes/UINode.h"
#include "ModelBase/src/nodes/nodeMacros.h"
#include "ModelBase/src/nodes/TypedList.h"
#include "ModelBase/src/nodes/Node.h"

namespace Visualization {
	class ViewItemNode;
}
extern template class VISUALIZATIONBASE_API Model::TypedList<Visualization::ViewItemNode>;

namespace Visualization {

class ViewItem;

/**
 * The ViewItemNode class is used in the ViewItem class and simply wraps a top level node
 * to give another level of indirection. This helps with distinguishing different items when
 * the same node is added to a ViewItem multiple times, as is possible.
 *
 * A ViewItemNode can either hold a reference to another node - in which case its visualization renders that node.
 * It can also hold a spacing target and spacing parent - in which case it will render as white space,
 * aligning its bottom end with the top end of that given spacing target.
 */
class VISUALIZATIONBASE_API ViewItemNode : public Super<UINode>
{
		NODE_DECLARE_STANDARD_METHODS(ViewItemNode)

	public:
		/**
		 * Creates a new ViewItemNode which is used to render an empty spacing item.
		 */
		static ViewItemNode* withSpacingTarget(Model::Node* spacingTarget, ViewItemNode* spacingParent);
		/**
		 * Creates a new ViewItemNode which is used to render the given reference's visualization.
		 */
		static ViewItemNode* withReference(Model::Node* reference, int purpose);
		/**
		 * Creates a new ViewItemNode using the given Json object for initialization. This can
		 * then be either a ViewItemNode with a reference or a spacing target.
		 */
		static ViewItemNode* fromJson(QJsonObject json, const ViewItem* view);

		void setReference(Model::Node* reference);
		Model::Node* reference() const;
		void setPurpose(int purpose);
		int purpose() const;

		void setSpacingTarget(Model::Node* spacingTarget);
		Model::Node* spacingTarget() const;
		void setSpacingParent(ViewItemNode* spacingParent);
		ViewItemNode* spacingParent() const;

		/**
		 * Use setPosition to set the position of this node in the ViewItem's grid before calling toJson.
		 */
		void setPosition(QPoint pos);
		/**
		 * If a spacing parent exists, use this to set the position of its spacing parent's position
		 * in the ViewItem's grid before calling toJson.
		 */
		void setSpacingParentPosition(QPoint pos);
		virtual QJsonValue toJson() const override;

	private:
		Model::Node* reference_{};
		int purpose_{-1};
		Model::Node* spacingTarget_{};
		ViewItemNode* spacingParent_{};

		QPoint position_;
		QPoint spacingParentPosition_;
};

inline void ViewItemNode::setReference(Model::Node *reference) { reference_ = reference; }
inline Model::Node* ViewItemNode::reference() const { return reference_; }
inline void ViewItemNode::setPurpose(int purpose) { purpose_ = purpose; }
inline int ViewItemNode::purpose() const { return purpose_; }
inline void ViewItemNode::setSpacingTarget(Model::Node* spacingTarget) { spacingTarget_ = spacingTarget; }
inline Model::Node* ViewItemNode::spacingTarget() const { return spacingTarget_; }
inline void ViewItemNode::setSpacingParent(ViewItemNode *spacingParent) { spacingParent_ = spacingParent; }
inline ViewItemNode* ViewItemNode::spacingParent() const { return spacingParent_; }
inline void ViewItemNode::setPosition(QPoint pos) { position_ = pos; }
inline void ViewItemNode::setSpacingParentPosition(QPoint pos) { spacingParentPosition_ = pos; }

}
