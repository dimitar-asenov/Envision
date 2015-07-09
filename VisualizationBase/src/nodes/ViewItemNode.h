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

#include "../VisualizationBase/src/visualizationbase_api.h"
#include "../VisualizationBase/src/nodes/UINode.h"

namespace Visualization {

/**
 * The ViewItemNode class is used in the ViewItem class and simply wraps a top level node
 * to give another level of indirection. This helps with distinguishing different items when
 * the same node is added to a ViewItem multiple times, as is possible.
 */
class VISUALIZATIONBASE_API ViewItemNode : public Super<UINode>
{
	public:
		static ViewItemNode* withSpacingTarget(Model::Node* spacingTarget, ViewItemNode* spacingParent);
		static ViewItemNode* withReference(Model::Node* reference, int purpose);

		void setReference(Model::Node* reference);
		Model::Node* reference() const;
		void setPurpose(int purpose);
		int purpose() const;

		void setSpacingTarget(Model::Node* spacingTarget);
		Model::Node* spacingTarget() const;
		void setSpacingParent(ViewItemNode* spacingParent);
		ViewItemNode* spacingParent() const;

	private:
		Model::Node* reference_{};
		int purpose_{-1};
		Model::Node* spacingTarget_{};
		ViewItemNode* spacingParent_{};

};

inline void ViewItemNode::setReference(Model::Node *reference) { reference_ = reference; }
inline Model::Node* ViewItemNode::reference() const { return reference_; }
inline void ViewItemNode::setPurpose(int purpose) { purpose_ = purpose; }
inline int ViewItemNode::purpose() const { return purpose_; }
inline void ViewItemNode::setSpacingTarget(Model::Node* spacingTarget) { spacingTarget_ = spacingTarget; }
inline Model::Node* ViewItemNode::spacingTarget() const { return spacingTarget_; }
inline void ViewItemNode::setSpacingParent(ViewItemNode *spacingParent) { spacingParent_ = spacingParent; }
inline ViewItemNode* ViewItemNode::spacingParent() const { return spacingParent_; }

}
