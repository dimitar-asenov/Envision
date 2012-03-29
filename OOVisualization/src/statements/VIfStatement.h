/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * VIfStatement.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VIFSTATEMENT_H_
#define VIFSTATEMENT_H_

#include "../oovisualization_api.h"
#include "VIfStatementStyle.h"

#include "OOModel/src/statements/IfStatement.h"

#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/items/LayoutProvider.h"

namespace Visualization {
	class PanelBorderLayout;
	class PositionLayout;
	class SequentialLayout;
	class VList;
}

namespace OOVisualization {

class OOVISUALIZATION_API VIfStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::IfStatement>
{
	ITEM_COMMON(VIfStatement)

	public:
		VIfStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VIfStatement();

		Visualization::SequentialLayout* header() const;
		Visualization::SequentialLayout* conditionBackground() const;
		Visualization::Item* condition() const;
		Visualization::SequentialLayout* content() const;
		Visualization::VList* thenBranch() const;
		Visualization::VList* elseBranch() const;

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header_;
		Visualization::SequentialLayout* conditionBackground_;
		Visualization::Item* condition_;
		Visualization::SequentialLayout* content_;
		Visualization::VList* thenBranch_;
		Visualization::VList* elseBranch_;
};

inline Visualization::SequentialLayout* VIfStatement::header() const { return header_; }
inline Visualization::SequentialLayout* VIfStatement::conditionBackground() const { return conditionBackground_; }
inline Visualization::Item* VIfStatement::condition() const { return condition_; }
inline Visualization::SequentialLayout* VIfStatement::content() const { return content_; }
inline Visualization::VList* VIfStatement::thenBranch() const { return thenBranch_; }
inline Visualization::VList* VIfStatement::elseBranch() const { return elseBranch_; }

}

#endif /* VIFSTATEMENT_H_ */
