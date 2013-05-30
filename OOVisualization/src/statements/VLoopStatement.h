/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "../oovisualization_api.h"
#include "VLoopStatementStyle.h"
#include "VStatementItem.h"

#include "OOModel/src/statements/LoopStatement.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.h"

namespace Visualization {
	class NodeWrapper;
	class Static;
}

namespace OOVisualization {

class VStatementItemList;

class OOVISUALIZATION_API VLoopStatement
	: public Super<VStatementItem<VLoopStatement, Visualization::DeclarativeItem<VLoopStatement>,
	  OOModel::LoopStatement>>
{
	ITEM_COMMON(VLoopStatement)

	public:
		VLoopStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		Visualization::NodeWrapper* condition() const;
		Visualization::NodeWrapper* initStep() const;
		Visualization::NodeWrapper* updateStep() const;
		VStatementItemList* body() const;
		Visualization::Static* icon() const;

		static void initializeForms();

	private:
		Visualization::NodeWrapper* condition_{};
		Visualization::NodeWrapper* initStep_{};
		Visualization::NodeWrapper* updateStep_{};
		VStatementItemList* body_{};
		Visualization::Static* icon_{};
};

inline Visualization::NodeWrapper* VLoopStatement::condition() const { return condition_; }
inline Visualization::NodeWrapper* VLoopStatement::initStep() const { return initStep_; }
inline Visualization::NodeWrapper* VLoopStatement::updateStep() const { return updateStep_; }
inline VStatementItemList* VLoopStatement::body() const { return body_; }
inline Visualization::Static* VLoopStatement::icon() const {return icon_;}

}
