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
#include "VForEachStatementStyle.h"
#include "VStatementItem.h"

#include "OOModel/src/statements/ForEachStatement.h"

#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.h"

namespace Visualization {
	class Static;
	class VText;
	class NodeWrapper;
}

namespace OOVisualization {

class OOVISUALIZATION_API VForEachStatement
	: public Super<VStatementItem<VForEachStatement, Visualization::DeclarativeItem<VForEachStatement>,
	  OOModel::ForEachStatement>>
{
	ITEM_COMMON(VForEachStatement)

	public:
		VForEachStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		Visualization::VText* varName() const;
		Visualization::NodeWrapper* collection() const;
		Visualization::NodeWrapper* varType() const;
		Visualization::Item* body() const;
		Visualization::Static* icon() const;

		static void initializeForms();

	private:

		Visualization::Static* icon_{};
		Visualization::VText* varName_{};
		Visualization::NodeWrapper* collection_{};
		Visualization::NodeWrapper* varType_{};
		Visualization::Item* body_{};
};

inline Visualization::VText* VForEachStatement::varName() const { return varName_; }
inline Visualization::NodeWrapper* VForEachStatement::collection() const { return collection_; }
inline Visualization::NodeWrapper* VForEachStatement::varType() const { return varType_; }
inline Visualization::Item* VForEachStatement::body() const { return body_; }
inline Visualization::Static* VForEachStatement::icon() const {return icon_;}

}
