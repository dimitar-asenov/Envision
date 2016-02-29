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

#include "VFormalArgument.h"

#include "OOModel/src/expressions/LambdaExpression.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VFormalArgument, "item")

VFormalArgument::VFormalArgument(Item* parent, NodeType* node, const StyleType* style) :
	Super{parent, node, style}
{}

void VFormalArgument::initializeForms()
{
	auto nameEl = item<VText>(&I::name_, [](I* v){return v->node()->nameNode();},
			[](I* v){return &v->style()->name();});

	auto outIconEl = item<Static>(&I::icon_, [](I* v){return &v->style()->outIcon();})
								->setEnabled([](I* v) -> bool {return v->node()->direction()==FormalArgument::OUT;})
								->setRightMargin(4);

	auto typeEl = item(&I::type_, [](I* v){return v->node()->typeExpression();});

	addForm(grid({
			{nameEl, outIconEl},
			{Merge{typeEl, 2, 1}}
		})
		->setHorizontalAlignment(LayoutStyle::Alignment::Center)
		->setNoInnerCursors([](Item*){return true;})
		->setNoBoundaryCursors([](Item*){return true;})
	);

	addForm(grid({	{typeEl, nameEl, outIconEl} })
		->setNoInnerCursors([](Item*){return true;})
		->setNoBoundaryCursors([](Item*){return true;})
	);
}

int VFormalArgument::determineForm()
{
	if (node()->parent() && node()->parent()->parent() && node()->parent()->parent()->typeId()
			== OOModel::LambdaExpression::typeIdStatic()) return 1;
	else return 0;
}

}
