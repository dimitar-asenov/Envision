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
#include "VMethodStyle.h"

#include "OOModel/src/top_level/Method.h"

#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/items/LayoutProvider.h"

namespace Visualization {
	class VText;
	class VList;
	class Static;
	class PanelBorderLayout;
	class Line;
}

namespace OOVisualization {

class VStatementItemList;

class OOVISUALIZATION_API VMethod
: public Super<Visualization::ItemWithNode<VMethod, Visualization::LayoutProvider<Visualization::PanelBorderLayout>,
  OOModel::Method>>
{
	ITEM_COMMON(VMethod)

	public:
		VMethod(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VMethod();

		Visualization::VText* name() const;
		VStatementItemList* body() const;
		VStatementItemList* annotations() const;
		Visualization::SequentialLayout* content() const;
		Visualization::VList* typeArguments() const;
		Visualization::VList* arguments() const;

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header_{};
		Visualization::Static* icon_{};
		Visualization::VText* name_{};
		Visualization::VList* typeArguments_{};
		Visualization::VList* arguments_{};
		Visualization::Line* signatureLine_{};
		VStatementItemList* body_{};
		VStatementItemList* annotations_{};
		Visualization::SequentialLayout* addons_{};
		Visualization::SequentialLayout* content_{};
		Visualization::VList* results_{};
};

inline Visualization::VText* VMethod::name() const { return name_; }
inline VStatementItemList* VMethod::body() const { return body_; }
inline VStatementItemList* VMethod::annotations() const { return annotations_; }
inline Visualization::SequentialLayout* VMethod::content() const { return content_; }
inline Visualization::VList* VMethod::typeArguments() const { return typeArguments_; }
inline Visualization::VList* VMethod::arguments() const { return arguments_; }

}
