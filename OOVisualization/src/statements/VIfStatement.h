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

#pragma once

#include "../oovisualization_api.h"
#include "VIfStatementStyle.h"
#include "VStatementItem.h"

#include "OOModel/src/statements/IfStatement.h"

#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.h"

namespace Visualization {
	class SequentialLayout;
	class Static;
	class NodeWrapper;
	class Line;
}

namespace OOVisualization {

class VStatementItemList;

class OOVISUALIZATION_API VIfStatement
	: public Super<VStatementItem<VIfStatement, Visualization::DeclarativeItem<VIfStatement>,
	  OOModel::IfStatement>>
{
	ITEM_COMMON(VIfStatement)

	public:
		VIfStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		Visualization::NodeWrapper* condition() const;
		VStatementItemList* thenBranch() const;
		VStatementItemList* elseBranch() const;
		Visualization::Static* icon() const;
		Visualization::Static* elseIcon() const;

		static void initializeForms();
		virtual int determineForm() override;

	private:
		Visualization::NodeWrapper* condition_{};
		VStatementItemList* thenBranch_{};
		VStatementItemList* elseBranch_{};
		Visualization::Static* icon_{};
		Visualization::Static* elseIcon_{};
		Visualization::Line* elseLine_{};
};

inline Visualization::NodeWrapper* VIfStatement::condition() const { return condition_; }
inline VStatementItemList* VIfStatement::thenBranch() const { return thenBranch_; }
inline VStatementItemList* VIfStatement::elseBranch() const { return elseBranch_; }
inline Visualization::Static* VIfStatement::icon() const {return icon_;}
inline Visualization::Static* VIfStatement::elseIcon() const {return elseIcon_;}

}
